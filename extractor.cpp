#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

using namespace std;

void mainfunc(const string& filename);
vector<unsigned char> decompress(vector<unsigned char>& a, int b);
string readcstr(ifstream& file);
unsigned int readuint32(ifstream& file);
void mkDIR(const string& dir);

void mainfunc(const string& filename)
{
    string folder = filename.substr(0, filename.find("."));
    mkDIR(folder);
    ifstream file(filename, ios::binary);
    if (!file.is_open())
    {
        cout << "Failed to open file: " << filename << endl;
        return;
    }
    file.seekg(0, ios::end);
    streampos end = file.tellg();
    file.seekg(0);
    while (file.tellg() < end)
    {
        string name = readcstr(file);
        [[maybe_unused]] string ftype = readcstr(file);
        unsigned int c = readuint32(file);
        unsigned int d = readuint32(file);
        unsigned int e = readuint32(file);
        vector<unsigned char> data(d);
        file.read(reinterpret_cast<char*>(data.data()), d);
        if (c & 1)
        {
            data = decompress(data, e);
        }
        ofstream output(folder + "/" + name, ios::binary);
        if (!output.is_open())
        {
            cout << "Failed to open output file: " << folder << "/" << name << "\n";
            continue;
        }
        output.write(reinterpret_cast<const char*>(data.data()), data.size());
        output.close();
    }
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
vector<unsigned char> decompress(vector<unsigned char>& a, int b)
{
    vector<unsigned char>c(b);
    int d = 0;
    unsigned int e[4096];
    unsigned int f[4096];
    int g = 256;
    int h = a.size();
    int k = 0;
    int l = 1;
    int m = 0;
    int n = 1;
    c[d] = a[0];
    d++;
    int r = 1;
    while (true)
    {
        n = r + (r >> 1);
        if ((n+1)>=h)
        {
            break;
        }
        m = a[n + 1];
        n = a[n];
        int p = (r & 1) ? (m << 4 | n >> 4) : ((m & 15) << 8 | n);
        if (p < g)
        {
            if (256 > p)
            {
                m = d;
                n = 1;
                c[d] = p;  // NOLINT(clang-diagnostic-implicit-int-conversion)
                d++;
            }
            else
            {
                m = d;
                n = f[p];  // NOLINT(bugprone-narrowing-conversions)
                p = e[p];  // NOLINT(bugprone-narrowing-conversions)
                int q = p + n;
                while (p < q)
                {
                    c[d] = c[p];
                    d++;
                    p++;
                }

            }
        }
        else if (p == g)
        {
            m = d;
            n = l + 1;
            p = k;
            int q = k + l;
            while (p < q)
            {
                c[d] = c[p];
                d++;
                p++;
            }
            c[d] = c[k];
            d++;
        }
        else
        {
            break;
        }
        e[g] = k;
        f[g] = l + 1;
        g++;
        k = m;
        l = n;
        g = (4096 <= g) ? 256 : g;
        r++;
    }
    return (d == b) ? c : vector<unsigned char>();
}

string readcstr(ifstream& file)
{
    string str;
    char c;
    while (file.get(c) && c != '\0')
    {
        str += c;
    }
    return str;
}

unsigned int readuint32(ifstream& file)
{
    unsigned char buffer[4];
    file.read(reinterpret_cast<char*>(buffer), 4);
    unsigned int result = (static_cast<unsigned int>(buffer[0])) | (static_cast<unsigned int>(buffer[1]) << 8) | (
                              static_cast<unsigned int>(buffer[2]) << 16) | (static_cast<unsigned int>(buffer[3]) << 24);
    return result;
}

void mkDIR(const string& dir)
{
    if (!filesystem::exists(dir))
    {
        if (!filesystem::create_directory(dir))
        {
            cout << "Error: Cannot create directory: (" << dir << ").\n";
        }
    }
}
