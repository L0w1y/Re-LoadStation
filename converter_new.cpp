#include <converter_new.h>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QJsonDocument>
#include <QVector>
#include <QDebug>

void writeObj(QFile& outputFile, const QString& name, const QVector<QVector<int>>& faces, const QVector<QVector<float>>& vertices, const QVector<QVector<float>>& uvs, const QString& material)
{
    outputFile.write("mtllib master.mtl\n");

    QVector<float> v = vertices[0];
    QVector<float> vt = uvs[0];

    for (int i = 0; i < v.size(); i += 3) {
        outputFile.write(QString("v %1 %2 %3\n").arg(v.at(i)).arg(v.at(i+1)).arg(v.at(i+2)).toUtf8());
    }

    for (int i = 0; i < vt.size(); i += 2) {
        outputFile.write(QString("vt %1 %2\n").arg(vt.at(i)).arg(vt.at(i+1)).toUtf8());
    }

    outputFile.write(QString("g %1\n").arg(name).toUtf8());
    outputFile.write(QString("usemtl %1\n").arg(material).toUtf8());

    for (const auto& face : faces) {
        outputFile.write(QString("f %1/%1 %2/%2 %3/%3\n")
                             .arg(face.at(0)+1)
                             .arg(face.at(1)+1)
                             .arg(face.at(2)+1).toUtf8());
    }
}

void convertScene(const QString& folder)
{
    QFile inputFile(QString("%1/scene.json").arg(folder));
    inputFile.open(QIODevice::ReadOnly);

    QByteArray data = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    inputFile.close();

    QFile outputMtl(QString("%1/master.mtl").arg(folder));
    outputMtl.open(QIODevice::WriteOnly | QIODevice::Text);

    auto materialsArray = json.value(QString("materials")).toArray();

    for (const auto& material : materialsArray) {
        auto name = material.toObject().value(QString("name")).toString();
        auto diffuse = material.toObject().value(QString("albedoTex")).toString();
        outputMtl.write(QString("newmtl %1\n").arg(name).toUtf8());
        outputMtl.write(QString("map_Ka %1\n").arg(diffuse).toUtf8());
        outputMtl.write(QString("map_Kd %1\n").arg(diffuse).toUtf8());
    }

    outputMtl.close();

    auto meshes = json.value(QString("meshes")).toArray();

    for (const auto& mesh : meshes) {
        auto name = mesh.toObject().value(QString("name")).toString();
        auto dat = mesh.toObject().value(QString("file")).toString();
        qDebug() << "converting" << dat;

        QFile inputFile(QString("%1/%2").arg(folder).arg(dat));
        inputFile.open(QIODevice::ReadOnly);

        auto wireCount = mesh.toObject().value(QString("wireCount")).toInt();
        auto indexCount = mesh.toObject().value(QString("indexCount")).toInt();
        auto vertexCount = mesh.toObject().value(QString("vertexCount")).toInt();
        auto texCoord2 = mesh.toObject().value(QString("secondaryTexCoord")).toInt();
        auto vertexColor = mesh.toObject().value(QString("vertexColor")).toInt();
        auto indexTypeSize = mesh.toObject().value(QString("indexTypeSize")).toInt();
        auto stride = 32;
        if (vertexColor > 0) {
            stride += 4;
        }
        if (texCoord2 > 0) {
            stride += 8;
        }

        QVector<QVector<int>> faceList;
        QVector<QVector<float>> vertList;
        QVector<QVector<float>> uvList;
        QVector<QString> materialsList;

        auto subMeshes = mesh.toObject().value(QString("subMeshes")).toArray();

        int offset = wireCount * indexTypeSize;

        for (int i = 0; i < subMeshes.size(); i++) {
            auto subMesh = subMeshes[i].toObject();

            auto material = subMesh.value(QString("material")).toString();
            auto indexCount2 = subMesh.value(QString("indexCount")).toInt();
            auto wireCount2 = subMesh.value(QString("wireIndexCount")).toInt();
            auto faceCount = int((indexCount2 * indexTypeSize) / 6);
            if (indexTypeSize == 4) {
                faceCount = int((indexCount2 * indexTypeSize) / 12);
            }

            QVector<QVector<int>> subFaceList;

            for (int j = 0; j < faceCount; j++) {
                QVector<int> face;
                if (indexTypeSize == 2) {
                    QDataStream stream(&inputFile);
                    stream.setByteOrder(QDataStream::LittleEndian);
                    for (int k = 0; k < 3; k++) {
                        quint16 index;
                        stream >> index;
                        face.append(index);
                    }
                } else {
                    QDataStream stream(&inputFile);
                    stream.setByteOrder(QDataStream::LittleEndian);
                    for (int k = 0; k < 3; k++) {
                        quint32 index;
                        stream >> index;
                        face.append(index);
                    }
                }
                subFaceList.append(face);
            }
            faceList.append(subFaceList);
            materialsList.append(material);
        }

        inputFile.seek(inputFile.pos()+offset);

        for (int i = 0; i < vertexCount; i++) {
            QVector<float> vert(3);
            QVector<float> uv(2);

            inputFile.read(reinterpret_cast<char*>(vert.data()), 12);
            inputFile.read(reinterpret_cast<char*>(uv.data()), 8);

            inputFile.seek(inputFile.pos() + stride - 20);

            vertList.append(vert);
            uvList.append(uv);
        }

        QFile outputObj(QString("%1/%2.obj").arg(folder).arg(dat));
        outputObj.open(QIODevice::WriteOnly | QIODevice::Text);

        for (int i = 0; i < subMeshes.size(); i++) {
            writeObj(outputObj, QString("%1_sub%2").arg(name).arg(i), faceList.at(i), vertList, uvList, materialsList.at(i));
        }
        inputFile.close();
        outputObj.close();
    }

    qDebug() << "COMPLETED!!!";
}

void mkDIR(const QString& dir)
{
    if (!QDir(dir).exists()) {
        QDir().mkpath(dir);
    }
}
