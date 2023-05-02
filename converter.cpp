#include <converter.h>

converter::converter(){}

converter::~converter(){}

void converter::convert(QString folder) {
    qDebug() << QString("Load folder %1").arg(folder);
    QFile f(QString("%1/scene.json").arg(folder));
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonDocument data = QJsonDocument::fromJson(f.readAll());
    qDebug() << "Created JsonDocument";
    f.close();
    QFile omtl(QString("%1/master.mtl").arg(folder));
    if (!omtl.open(QIODevice::WriteOnly)) {
        return;
    }
    qDebug() << "Extract materials";
    QTextStream omtlStream(&omtl);
    QJsonArray materials = data.object()["materials"].toArray();
    for (auto matVal : materials) {
        QJsonObject mat = matVal.toObject();
        QString name = mat["name"].toString();
        QString diffuse = mat["albedoTex"].toString();
        omtlStream << QString("newmtl %1\n").arg(name);
        omtlStream << QString("map_Ka %1\n").arg(diffuse);
        omtlStream << QString("map_Kd %1\n").arg(diffuse);
        qDebug() << QString("Extract %1 material").arg(mat["name"].toString());
    }
    omtl.close();
    QJsonArray meshes = data.object()["meshes"].toArray();
    for (auto meshVal : meshes) {
        QJsonObject mesh = meshVal.toObject();
        QString name = mesh["name"].toString();
        QString dat = mesh["file"].toString();
        qDebug().noquote() << "converting" << dat;

        int wireCount = mesh["wireCount"].toInt();
        int indexCount = mesh["indexCount"].toInt();
        int vertexCount = mesh["vertexCount"].toInt();
        int texCoord2 = mesh.contains("secondaryTexCoord") ? mesh["secondaryTexCoord"].toInt() : 0;
        int vertexColor = mesh.contains("vertexColor") ? mesh["vertexColor"].toInt() : 0;
        int indexTypeSize = mesh["indexTypeSize"].toInt();
        int stride = 32;
        if (vertexColor > 0) stride += 4;
        if (texCoord2 > 0) stride += 8;

        QFile df(QString("%1/%2").arg(folder, dat));
        if (!df.open(QIODevice::ReadOnly)) {
            continue;
        }
        QTextStream outputStream(new QFile(QString("%1/%2.obj").arg(folder, dat)));
        outputStream << "mtllib master.mtl\n";
        QList<QList<QVector3D>> faceList;
        QList<int> materialsList;
        QJsonArray subMeshes = mesh["subMeshes"].toArray();
        for (auto subMeshVal : subMeshes) {
            QJsonObject subMesh = subMeshVal.toObject();
            QList<QVector3D> faces;
            int material = subMesh["material"].toInt();
            int indexCount2 = subMesh["indexCount"].toInt();
            int wireIndexCount = subMesh["wireIndexCount"].toInt();
            int faceCount = (indexCount2 * indexTypeSize) / 6;
            if (indexTypeSize == 4)
                faceCount = (indexCount2 * indexTypeSize) / 12;

            char faceBuffer[12];
            for (int i = 0; i < faceCount; ++i) {
                if (indexTypeSize == 2) {
                    df.read(faceBuffer, 6);
                    QVector3D face(ushort(faceBuffer[0]) + 1, ushort(faceBuffer[2]) + 1, ushort(faceBuffer[4]) + 1);
                    faces.append(face);
                } else {
                    df.read(faceBuffer, 12);
                    QVector3D face(uint(faceBuffer[0]) + 1, uint(faceBuffer[4]) + 1, uint(faceBuffer[8]) + 1);
                    faces.append(face);
                }
            }
            faceList.append(faces);
            materialsList.append(material);
        }
        df.seek(wireCount * indexTypeSize);
        QList<QVector3D> vertList;
        QList<QVector2D> uvList;
        for (int i = 0; i < vertexCount; ++i) {
            char vertexBuffer[32];
            df.read(vertexBuffer, stride);
            QVector3D pos(reinterpret_cast<float*>(vertexBuffer)[0], reinterpret_cast<float*>(vertexBuffer)[1], reinterpret_cast<float*>(vertexBuffer)[2]);
            QVector2D texpos(reinterpret_cast<float*>(vertexBuffer)[3], reinterpret_cast<float*>(vertexBuffer)[4]);
            vertList.append(pos);
            uvList.append(texpos);
        }
        df.close();
        for (QVector3D vert : vertList) {
            outputStream << QString("v %1 %2 %3\n").arg(vert.x()).arg(vert.y()).arg(vert.z());
        }
        for (QVector2D uv : uvList) {
            outputStream << QString("vt %1 %2\n").arg(uv.x()).arg(uv.y());
        }
        int faceListIndex = 0;
        for (QList<QVector3D> faces : faceList) {
            outputStream << '\n';
            outputStream << QString("g %1\n").arg(name);
            outputStream << QString("usemtl %1\n").arg(materialsList[faceListIndex++]);
            for (QVector3D face : faces) {
                outputStream << QString("f %1/%1/%1 %2/%2/%2 %3/%3/%3\n").arg(static_cast<int>(face.x())).arg(static_cast<int>(face.y())).arg(static_cast<int>(face.z()));
            }
        }
//        qDebug() << "converted" << dat;
    }
//    qDebug() << "COMPLETED!!!";
}

void converter::mkDIR(const QString &dir) {
    QDir().mkpath(dir);
}
