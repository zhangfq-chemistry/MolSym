#include "mTools.h"

#include<cmath>
using namespace std;


vector3 buildColor(const QColor& clr)
{
    double r = (double)clr.red() / 255;
    double g = (double)clr.green() / 255;
    double b = (double)clr.blue() / 255;
    vector3 v (r,g,b);
    return v;
}

uint32_t buildColor1(vector3 v)
{
    unsigned int  r,g,b;

    if( v.x()<1.0 && v.y()<1.0  && v.z()<1.0) {
        r = round(v.x()*255);
        g = round(v.y()*255);
        b = round(v.z()*255);
    }
    else {
        r = round(v.x());
        g = round(v.y());
        b = round(v.z());
    }

    uint32_t dw = 0;
    dw+=(r<<16);
    dw+=(g<<8);
    dw+=(b);
    return dw;
}

uint32_t dw=0;



vector3 MakeGlColor(uint32_t dw)
{
   float v1,v2,v3;
   float &r = v1, &g = v2, &b = v3;
   r = (1.f/255.f)*((dw>>16)&0xff);
   g = (1.f/255.f)*((dw>>8)&0xff);
   b = (1.f/255.f)*((dw>>0)&0xff);

   vector3 v (r,g,b);

   return v;
}

bool cmpPair(pair<int, double>a, pair<int, double>b)
{
    return a.second < b.second;
}

unsigned int Symbol2Z(QString symbol)
{
    int Z=1;

    if (symbol=="X" || symbol=="Bq"){
        Z=0; return Z;
    }

    if (symbol=="H")  Z=1;
    if (symbol=="Be") Z=4;
    if (symbol=="B")  Z=5;
    if (symbol=="C")  Z=6;
    if (symbol=="N")  Z=7;
    if (symbol=="O")  Z=8;
    if (symbol=="F")  Z=9;

    if (symbol=="Si")  Z=14;
    if (symbol=="P")   Z=15;
    if (symbol=="S")   Z=16;
    if (symbol=="Cl")  Z=17;

    if (symbol=="Ti")  Z=22;
    if (symbol=="V")   Z=23;
    if (symbol=="Cr")  Z=24;
    if (symbol=="Mn")  Z=25;
    if (symbol=="Fe")  Z=26;
    if (symbol=="Co")  Z=27;
    if (symbol=="Ni")  Z=28;
    if (symbol=="Cu")  Z=29;
    if (symbol=="Zn")  Z=30;
    if (symbol=="Ga")  Z=31;
    if (symbol=="Ge")  Z=32;
    if (symbol=="As")  Z=33;
    if (symbol=="Se")  Z=34;
    if (symbol=="Br")  Z=35;

    if (symbol=="Zr")  Z=40;
    if (symbol=="Nb")  Z=41;
    if (symbol=="Mo")  Z=42;

    if (symbol=="Ru")  Z=44;
    if (symbol=="Rh")  Z=45;
    if (symbol=="Pd")  Z=46;
    if (symbol=="Ag")  Z=47;
    if (symbol=="Cd")  Z=48;
    if (symbol=="In")  Z=49;
    if (symbol=="Sn")  Z=50;
    if (symbol=="Sb")  Z=51;
    if (symbol=="Te")  Z=52;
    if (symbol=="I")   Z=53;

    if (symbol=="Ta")  Z=73;
    if (symbol=="W")   Z=74;
    if (symbol=="Re")  Z=75;
    if (symbol=="Os")  Z=76;
    if (symbol=="Ir")  Z=77;
    if (symbol=="Pt")  Z=78;
    if (symbol=="Au")  Z=79;
    if (symbol=="Hg")  Z=80;

    return Z;
}

/*
double  CovalentRadii[120] = {0.1, 0.3,   0.7181,   0.6047,   2.5322,
   1.7008,   1.5496,   1.4551,   1.4173,   1.3795,   1.3417,   1.3039,   2.9102,
   2.4566,   2.2299,   2.0976,   2.0031,   1.9275,   1.8708,   1.8330,   3.7039,
   3.2881,   2.7212,   2.5700,   2.3622,   2.4000,   2.6267,   2.3622,   2.3811,
   2.2866,   2.6078,   2.4755,   2.3811,   2.3055,   2.2488,   2.1921,   2.1543,
   2.0787,   3.9873,   3.6283,   3.0614,   2.7968,   2.5889,   2.7401,   2.9480,
   2.3811,   2.5511,   2.4755,   2.8913,   2.7968,   2.7212,   2.6645,   2.6078,
   2.5511,   2.5133,   2.4566,   4.2519,   3.7417,   3.1936,   3.4355,   3.4469,
   3.4280,   3.4658,   3.4091,   3.4091,   3.4091,   3.3505,   3.3656,   3.3297,
   3.3278,   3.3240,   3.3259,   3.0236,   2.8346,   2.6078,   2.7590,   3.0047,
   2.4188,   2.5889,   2.4188,   2.7212,   2.8157,   2.7968,   2.7779,   2.7590,
   2.8300,   2.9200,   2.7401,   5.4400,   4.7500,   3.7500,   3.3826,   3.0803,
   2.9480,   2.9291,   3.0047,   3.2692,   3.2881,   3.2125,   3.5149,   3.5149,
   3.2400,   3.1900,   3.1700,   3.2100,   3.2000,   3.2000,   3.2000,   3.2000,
   3.2000,   3.2000,   3.2000,   3.2000,   3.2000,   3.2000,   3.2000,   3.2000,
   3.2000,   3.2000,   3.2000
};
*/
double  CovalentRadii[100] = {0.8, 0.6, 0.7,1.34,0.9, 0.82, 0.77, 0.75, 0.73, 0.71,
0.69, 1.54, 1.30, 1.18, 1.1, 1.06, 1.02, 0.99, 0.97,1.96,
1.73, 1.44, 1.36, 1.25, 1.27, 1.39, 1.25, 1.26, 1.21, 1.38,
1.31, 1.26, 1.22, 1.19, 1.16, 1.14, 1.10, 2.11,1.82, 1.62,
1.48, 1.37, 1.45, 1.56, 1.26, 1.35, 1.31, 1.53,1.48, 1.44,
1.41, 1.38, 1.35, 1.33, 1.30, 2.25, 1.98, 1.69,1.6, 1.6,
1.6,  1.6,  1.6,   1.6,  1.6, 1.6,   1.6,  1.6, 1.6, 1.6,
1.6,   1.6, 1.5,   1.38, 1.46, 1.59 ,1.28, 1.37,1.28,1.44,
1.49,  1.48,1.47,  1.46, 1.6, 1.6,   1.45, 1.6, 1.6, 1.6,
1.6,   1.6, 1.6,   1.6,  1.6,  1.6,   1.6, 1.6, 1.6,1.6};



string ElementNames[119] = {"X", "H", "He", "Li", "Be", "B", "C", "N",
   "O", "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
   "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As",
   "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd",
   "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr",
   "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf",
   "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At",
   "Rn", "Fr", "Ra", "Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf",
   "Es", "Fm", "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg",
   "Cn", "Uut", "Uuq", "Uup", "Uuh", "Uus", "Uuo"
};



double elementMasses[119] = {0.0, 1.00794, 4.002602, 6.941,
   9.012182, 10.811, 12.0107, 14.0067, 15.9994, 18.9984032, 20.1797,
   22.98976928, 24.3050, 26.9815386, 28.0855, 30.973762, 32.065, 35.453, 39.948,
   39.0983, 40.078, 44.955912, 47.867, 50.9415, 51.9961, 54.938045, 55.845,
   58.933195, 58.6934, 63.546, 65.38, 69.723, 72.64, 74.92160, 78.96, 79.904,
   83.798, 85.4678, 87.62, 88.90585, 91.224, 92.90638, 95.96, 98, 101.07,
   102.90550, 106.42, 107.8682, 112.411, 114.818, 118.710, 121.760, 127.60,
   126.90447, 131.293, 132.9054519, 137.327, 138.90547, 140.116, 140.90765,
   144.242, 145, 150.36, 151.964, 157.25, 158.92535, 162.500, 164.93032,
   167.259, 168.93421, 173.054, 174.9668, 178.49, 180.94788, 183.84, 186.207,
   190.23, 192.217, 195.084, 196.966569, 200.59, 204.3833, 207.2, 208.98040,
   209, 210, 222, 223, 226, 227, 232.03806, 231.03588, 238.02891, 237, 244, 243,
   247, 247, 251, 252, 257, 258, 259, 262, 265, 268, 271, 272, 270, 276, 281,
   280, 285, 284, 289, 288, 293, 292, 294
};

uint32_t ElementColors[110] = {0xdd00cc,
     0xFFFFFF,   0xD9FFFF,   0xCC80FF,  0xC2FF00,	 0xFFB5B5,
     0x909090,   0x3050F8,   0xFF0D0D,  0x90E050,    0xB3E3F5,
     0xAB5CF2,   0x8AFF00,   0xBFA6A6,   0xF0C8A0,   0xFF8000,
     0xFFFF30,	 0x1FF01F,	 0x80D1E3,	 0x8F40D4,	 0x3DFF00,
     0xE6E6E6,	 0xBFC2C7,	 0xA6A6AB,	 0x8A99C7,	 0x9C7AC7,
     0xE06633,	 0xF090A0,	 0x50D050,	 0xC88033,	 0x7D80B0,
     0xC28F8F,	 0x668F8F,	 0xBD80E3,	 0xFFA100,	 0xA62929,
     0x5CB8D1,	 0x702EB0,	 0x00FF00,	 0x94FFFF,	 0x94E0E0,
     0x73C2C9,	 0x54B5B5,	 0x3B9E9E,	 0x248F8F,	 0x0A7D8C,
     0x006985,	 0xC0C0C0,	 0xFFD98F,	 0xA67573,	 0x668080,
     0x9E63B5,	 0xD47A00,	 0x940094,	 0x429EB0,	 0x57178F,
     0x00C900,	 0x70D4FF,	 0xFFFFC7,	 0xD9FFC7,	 0xC7FFC7,
     0xA3FFC7,	 0x8FFFC7,	 0x61FFC7,	 0x45FFC7,	 0x30FFC7,
     0x1FFFC7,	 0x00FF9C,	 0x00E675,	 0x00D452,	 0x00BF38,
     0x00AB24,	 0x4DC2FF,	 0x4DA6FF,	 0x2194D6,	 0x267DAB,
     0x266696,	 0x175487,	 0xD0D0E0,	 0xFFD123,	 0xB8B8D0,
     0xA6544D,	 0x575961,	 0x9E4FB5,	 0xAB5C00,	 0x754F45,
     0x428296,	 0x420066,	 0x007D00,	 0x70ABFA,	 0x00BAFF,
     0x00A1FF,	 0x008FFF,	 0x0080FF,	 0x006BFF,	 0x545CF2,
     0x785CE3,	 0x8A4FE3,	 0xA136D4,	 0xB31FD4,	 0xB31FBA,
     0xB30DA6,	 0xBD0D87,	 0xC70066,	 0xCC0059,	 0xD1004F,
     0xD90045,	 0xE00038,   0xE6002E,	 0xEB0026
};

bool isNumber(const QString& src) {
    const char *s = src.toUtf8().data();
    while(*s && *s>='0' && *s<='9') s++;
    return !bool(*s);
}

bool isDigit(const QString& src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)//不是纯数字
    {
        return -1;
    }
    else//纯数字
    {
        return 0;
    }
}


bool isEnglish(QString &qstrSrc)
{
    QByteArray ba = qstrSrc.toLatin1();
    const char *s = ba.data();
    bool bret = true;
    while(*s) {
        if((*s>='A' && *s<='Z') || (*s>='a' && *s<='z'))     {}
        else   { bret = false; break; }
        s++;
    }
    return bret;
}

bool isEnglishOrNumber(QString &qstrSrc)
{
    QByteArray ba = qstrSrc.toLatin1();
    const char *s = ba.data(); bool bret = true;
    while(*s)
    {
       if((*s>='A' && *s<='Z') || (*s>='a' && *s<='z') || (*s>='0' && *s<='9')) { }
        else { bret = false; break; } s++;
    }
    return bret;
}


QString extractDigit(QString &str)
{
    QString tmp="";
    for(int j = 0; j < str.length(); j++)
        if(str[j] > '0' && str[j] < '9') tmp.append(str[j]);

    return tmp;

}

QString extractEnglish(QString &str)
{
    QString tmp="";
    for(int j = 0; j < str.length(); j++)   {
        if(str[j] > 'a' && str[j] < 'z') tmp.append(str[j]);
        if(str[j] > 'A' && str[j] < 'Z') tmp.append(str[j]);
    }

    return tmp;
}




bool isFloat(QString & src)
{
    string s=src.toStdString();
    std::istringstream iss(s);
    float f;
    iss >> noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

bool isInt(QString & str)
{
    bool ok=true;
    QString m_string = str.simplified();
    if (m_string.isEmpty()) return false;

    unsigned int length=m_string.length();
    for (int i=0;i<length;i++)
    {
        if(!m_string.at(i).isDigit())
        {
          ok=false;
          break;
        }
    }
    return ok;
}

