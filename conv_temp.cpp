#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>
#include <QDir>

// Функция конвертации
void Convert(QString folder) {
    // Открываем файл сцены

    QFile f(folder+"/scene.json");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file:" << f.errorString();
        return;
    }
    QByteArray data = f.readAll();
    f.close();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject json = jsonDoc.object();

    // Создаем файл материалов
    QFile omtl(folder+"/master.mtl");
    if (!omtl.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open file:" << omtl.errorString();
        return;
    }
    QJsonArray materials = json["materials"].toArray();

    // Для каждого материала в сцене записываем новый материал в .mtl файл
    for (int i = 0; i < materials.size(); ++i) {
        QJsonObject mat = materials[i].toObject();
        QString name = mat["name"].toString();
        QString diffuse = mat["albedoTex"].toString();
        omtl.write(QString("newmtl %1\n").arg(name).toUtf8());
        omtl.write(QString("map_Ka %1\n").arg(diffuse).toUtf8());
        omtl.write(QString("map_Kd %1\n").arg(diffuse).toUtf8());
    }
    omtl.close();

    QJsonArray meshes = json["meshes"].toArray();
    // Обрабатываем каждую сетку в сцене
    for (int i = 0; i < meshes.size(); ++i) {
        QJsonObject mesh = meshes[i].toObject();
        QString name = mesh["name"].toString();
        QString dat = mesh["file"].toString();
        qDebug() << "converting" << dat;
        int wire_count = mesh["wireCount"].toInt();
        int index_count = mesh["indexCount"].toInt();
        int vertex_count = mesh["vertexCount"].toInt();
        int tex_coord_2 = 0;
        if (mesh.contains("secondaryTexCoord")) {
            tex_coord_2 = mesh["secondaryTexCoord"].toInt();
        }
        int vertex_color = 0;
        if (mesh.contains("vertexColor")) {
            vertex_color = mesh["vertexColor"].toInt();
        }
        int index_type_size = mesh["indexTypeSize"].toInt();
        int stride = 32;
        if (vertex_color > 0) stride += 4;
        if (tex_coord_2 > 0) stride += 8;

        // Читаем файл .dat и записываем данные в .obj
        QFile df(QString("%1/%2").arg(folder).arg(dat));
        if (!df.open(QIODevice::ReadOnly)) {
            qDebug() << "Could not open file:" << df.errorString();
            return;
        }
        QFile output(QString("%1/%2.obj").arg(folder).arg(dat));
        if (!output.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open file:" << output.errorString();
            return;
        }
        output.write("mtllib master.mtl\n");
        QVector<QVector3D> vert_list;
        QVector<QVector2D> uv_list;
        QList<QList<QVector3D>> face_list;
        QList<QString> materials_list;
        QJsonArray sub_meshes = mesh["subMeshes"].toArray();

        // Для каждой под-сетки сетки
        for (int j = 0; j < sub_meshes.size(); ++j) {
            QList<QVector3D> faces;
            QJsonObject sub_mesh = sub_meshes[j].toObject();
            QString material = sub_mesh["material"].toString();
            int index_count_2 = sub_mesh["indexCount"].toInt();
            int wire_count_2 = sub_mesh["wireIndexCount"].toInt();
            int face_count = (index_count_2 * index_type_size) / 6;
            if (index_type_size == 4) {
                face_count = (index_count_2 * index_type_size) / 12;
            }

            // Записываем каждую грань
            for (int k = 0; k < face_count; ++k) {
                QByteArray faceData = df.read(index_type_size * 3);
                QVector3D face;
                if (index_type_size == 2) {
                    // Читаем грани из 2 байтов
                    quint16 *faceIndices = reinterpret_cast<quint16 *>(faceData.data());
                    face = QVector3D(faceIndices[0], faceIndices[1], faceIndices[2]);
                } else {
                    // Из 4-х байтов
                    quint32 *faceIndices = reinterpret_cast<quint32 *>(faceData.data());
                    face = QVector3D(faceIndices[0], faceIndices[1], faceIndices[2]);
                }
                faces.append(face);
            }
            face_list.append(faces);
            materials_list.append(material);
        }
        df.seek(wire_count * index_type_size);

        // Записываем каждую вершину и uv координаты
        for (int j = 0; j < vertex_count; ++j) {
            QVector3D pos;
            df.read(reinterpret_cast<char*>(&pos), sizeof(pos));
            QVector2D texpos;
            df.read(reinterpret_cast<char*>(&texpos), sizeof(texpos));
            df.seek(df.pos() + (stride - 20));
            vert_list.append(pos);
            uv_list.append(texpos);
        }

        // Записываем вершины в .obj файл
        for (const QVector3D &vert : vert_list) {
            output.write(QString("v %1 %2 %3\n").arg(vert.x()).arg(vert.y()).arg(vert.z()).toUtf8());
        }

        // Записываем uv координаты в .obj файл
        for (const QVector2D &uv : uv_list) {
            output.write(QString("vt %1 %2\n").arg(uv.x()).arg(uv.y()).toUtf8());
        }

        // Записываем грани, используя записанные вершины и uv координаты
        for (int j = 0; j < face_list.size(); ++j) {
            QList<QVector3D> faces = face_list[j];
            QString material = materials_list[j];
            output.write("\n");
            output.write(QString("g %1\n").arg(name).toUtf8());
            output.write(QString("usemtl %1\n").arg(material).toUtf8());
            for (const QVector3D &face : faces) {
                output.write(QString("f %1/%1/%1 %2/%2/%2 %3/%3/%3\n").arg(face.x() + 1).arg(face.y() + 1).arg(face.z() + 1).toUtf8());
            }
        }
        df.close();
        output.close();
    }
    qDebug() << "COMPLETED!!!";
}

// Функция создания директории, если ее не существует
void mkDIR(QString dir) {
    QDir().mkpath(dir);
}
