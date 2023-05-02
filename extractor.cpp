#include <extractor.h>

extractor::extractor() {}

extractor::~extractor() {}

void extractor::decompress(QByteArray &data, QByteArray &output, quint32 size) {
    quint32 i = 0;
    QByteArray c(size, '\0');
    QByteArray e(4096, '\0');
    quint32 d = 0;
    QList<quint32> f(4096, 0);
    quint32 g = 256;
    qint32 h = data.size();
    qint32 k = 0;
    quint32 l = 1;
    quint32 m = 0;
    quint32 n = 1;
    c[d++] = data[0];

    quint32 r = 1;
    while (true) {
        n = r + (r >> 1);
        if (n + 1 >= h)
            break;

        m = data[n + 1];
        n = data[n];
        quint32 p = (m << 4 | n >> 4) | ((r & 1) << 8 | ((m & 15) << 8 | n));
        if (p < g) {
            if (256 > p) {
                m = d;
                n = 1;
                c[d++] = static_cast<char>(p);
            } else {
                m = d;
                n = f[p];
                p = e[p];
                quint32 q = p + n;
                while (p < q) {
                    c[d++] = c[p++];
                }
            }
        } else if (p == g) {
            m = d;
            n = l + 1;
            p = k;
            quint32 q = k + l;
            while (p < q) {
                c[d++] = c[p++];
            }
            c[d++] = c[k];
        } else {
            break;
        }
        if (4096 <= g) {
            break;
        }

        e[g] = k;
        f[g] = l + 1;
        g++;
        k = m;
        l = n;
        r++;
    }
    output = c.left(size);
}

quint32 extractor::readuint32(QFile &file) {
    char buf[4];
    file.read(buf, 4);
    return *reinterpret_cast<quint32*>(buf);
}

QString extractor::readcstr(QFile &file) {
    QByteArray buf;
    while (true) {
        char b[1];
        file.read(b, 1);
        if (*b == 0) {
            break;
        } else {
            buf.append(b[0]);
        }
    }
    return QString::fromUtf8(buf.constData(), buf.size());
}

void extractor::mkDIR(const QString &dir) {
    QDir().mkpath(dir);
}

QString extractor::extract(QString filename) {
    QString folder = filename.split('.').first();
    extractor::mkDIR(folder);
    QFile bin(filename);
    if (!bin.open(QIODevice::ReadOnly)) {
        return QString();
    }
    bin.seek(0);
    qint64 end = bin.size();
    while (bin.pos() < end) {
        QString name = extractor::readcstr(bin);
        QString type = extractor::readcstr(bin);
        quint32 c = extractor::readuint32(bin);
        quint32 d = extractor::readuint32(bin);
        quint32 e = extractor::readuint32(bin);
        QByteArray data = bin.read(d);
        if (c & 1) {
            extractor::decompress(data, data, e);
        }
        QFile output(QString("%1/%2").arg(folder, name));
        if (output.open(QIODevice::WriteOnly)) {
            output.write(data);
            output.close();
        }
        qDebug() << name << type;
    }
    return folder;
}
