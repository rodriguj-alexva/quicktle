/*-----------------------------------------------------------------------------+
 | QuickTle                                                                    |
 | Copyright 2011-2015 Sergei Fundaev                                          |
 +-----------------------------------------------------------------------------+
 | This file is part of QuickTle library.                                      |
 |                                                                             |
 | QuickTle is free software: you can redistribute it and/or modify            |
 | it under the terms of the GNU Lesser General Public License as published by |
 | the Free Software Foundation, either version 3 of the License, or           |
 | (at your option) any later version.                                         |
 |                                                                             |
 | QuickTle is distributed in the hope that it will be useful,                 |
 | but WITHOUT ANY WARRANTY; without even the implied warranty of              |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
 | GNU Lesser General Public License for more details.                         |
 |                                                                             |
 | You should have received a copy of the GNU Lesser General Public License    |
 | along with QuickTle. If not, see <http://www.gnu.org/licenses/>.            |
 +----------------------------------------------------------------------------*/
/*!
    \file node.cpp
    \brief File contains the realization of methods of quicktle::Node class.
*/

#define CHECKSUM_INDEX 68  //!< Index of checksum symbol in the TLE format line

#define SECS_IN_DAY 86400
#define GM 3.986004418e14
#define E_RELATIVE_ERROR 1e-7

#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <quicktle/node.h>
#include <quicktle/func.h>

namespace quicktle
{

Node::Node()
{
    init();
    m_fileType = TwoLines;
}
//------------------------------------------------------------------------------

Node::Node(const std::string &line1, const std::string &line2,
           const std::string &line3, bool forceParsing)
{
    init();
    assign(line1, line2, line3, forceParsing);
}
//------------------------------------------------------------------------------

Node::Node(const std::string &line2, const std::string &line3,
           bool forceParsing)
{
    init();
    assign(line2, line3, forceParsing);
}
//------------------------------------------------------------------------------

Node::Node(const Node &node)
{
    init();
    m_line1 = node.m_line1;
    m_line2 = node.m_line2;
    m_line3 = node.m_line3;
    m_satelliteName = node.m_satelliteName;
    m_satelliteNumber = node.m_satelliteNumber;
    m_designator = node.m_designator;
    m_dn = node.m_dn;
    m_d2n = node.m_d2n;
    m_bstar = node.m_bstar;
    m_i = node.m_i;
    m_Omega = node.m_Omega;
    m_omega = node.m_omega;
    m_M = node.m_M;
    m_n = node.m_n;
    m_e = node.m_e;
    m_date = node.m_date;
    m_classification = node.m_classification;
    m_ephemerisType = node.m_ephemerisType;
    m_elementNumber = node.m_elementNumber;
    m_revolutionNumber = node.m_revolutionNumber;
    m_fileType = node.m_fileType;
    m_lastError = node.m_lastError;
    m_initList = node.m_initList;
}
//------------------------------------------------------------------------------

void Node::swap(Node& node)
{
    m_line1.swap(node.m_line1);
    m_line2.swap(node.m_line2);
    m_line3.swap(node.m_line3);

    m_satelliteName.swap(node.m_satelliteName);
    m_satelliteNumber.swap(node.m_satelliteNumber);
    m_designator.swap(node.m_designator);

    std::swap(m_dn, node.m_dn);
    std::swap(m_d2n, node.m_d2n);
    std::swap(m_bstar, node.m_bstar);
    std::swap(m_i, node.m_i);
    std::swap(m_Omega, node.m_Omega);
    std::swap(m_omega, node.m_omega);
    std::swap(m_M, node.m_M);
    std::swap(m_n, node.m_n);
    std::swap(m_e, node.m_e);
    std::swap(m_date, node.m_date);
    std::swap(m_classification, node.m_classification);
    std::swap(m_ephemerisType, node.m_ephemerisType);
    std::swap(m_elementNumber, node.m_elementNumber);
    std::swap(m_revolutionNumber, node.m_revolutionNumber);
    std::swap(m_fileType, node.m_fileType);

    std::swap(m_lastError, node.m_lastError);
    std::swap(m_initList, node.m_initList);
}
//------------------------------------------------------------------------------

Node& Node::operator=(Node node)
{
    swap(node);
    return *this;
}
//------------------------------------------------------------------------------

Node::~Node()
{
    free();
}
//------------------------------------------------------------------------------

void Node::init()
{
    m_dn = 0;
    m_d2n =  0;
    m_bstar = 0;
    m_i = 0;
    m_Omega = 0;
    m_omega = 0;
    m_M = 0;
    m_n = 0;
    m_e = 0;
    m_classification = '\0';
    m_ephemerisType = '\0';
    m_elementNumber = 0;
    m_revolutionNumber = 0;
    m_date = 0;
    m_lastError = NoError;
    m_initList.reset();
}
//------------------------------------------------------------------------------

void Node::free()
{
    m_line1.clear();
    m_line2.clear();
    m_line3.clear();
    m_satelliteName.clear();
    m_satelliteNumber.clear();
    m_designator.clear();
    init();
}
//------------------------------------------------------------------------------

bool Node::assign(const std::string &line1, const std::string &line2,
                  const std::string &line3, bool forceParsing)
{
    // Check checksums
    ErrorCode error = checkLine(line2);
    if (error != NoError)
    {
        m_lastError = error;
        return false;
    }

    error = checkLine(line3);
    if (error != NoError)
    {
        m_lastError = error;
        return false;
    }

    // Assign
    free();
    m_line1 = line1;
    m_line2 = line2;
    m_line3 = line3;
    m_fileType = ThreeLines;
    // Parse
    if (forceParsing)
        parseAll();

    return (m_lastError == NoError);
}
//------------------------------------------------------------------------------

bool Node::assign(const std::string &line2, const std::string &line3,
                  bool forceParsing)
{
    // Check checksums
    ErrorCode error = checkLine(line2);
    if (error != NoError)
    {
        m_lastError = error;
        return false;
    }

    error = checkLine(line3);
    if (error != NoError)
    {
        m_lastError = error;
        return false;
    }
    // Assign
    free();
    m_line2 = line2;
    m_line3 = line3;
    m_fileType = TwoLines;
    // Parse
    if (forceParsing)
        parseAll();

    return (m_lastError == NoError);
}
//------------------------------------------------------------------------------

void Node::parseAll()
{
    n();
    dn();
    d2n();
    i();
    Omega();
    omega();
    M();
    e();
    bstar();
    satelliteNumber();
    satelliteName();
    designator();
    classification();
    ephemerisType();
    elementNumber();
    revolutionNumber();
    preciseEpoch();
}
//------------------------------------------------------------------------------

Node::ErrorCode Node::checkLine(const std::string &str) const
{
    if (str.length() < CHECKSUM_INDEX + 1)
        return TooShortString;

    int expectedChecksum = checksum(str.substr(0, CHECKSUM_INDEX));
    int actualChecksum = atoi(str.substr(CHECKSUM_INDEX, 1).c_str());
    if (expectedChecksum != actualChecksum)
        return ChecksumError;

    return NoError;
}
//------------------------------------------------------------------------------

std::string Node::satelliteNumber() const
{
    if (m_initList.test(Field_SatNumber))
        return m_satelliteNumber;

    if (!m_line2.empty())
    {
        // Try to obtain the satellite number from the second line...
        ErrorCode error = NoError;
        m_satelliteNumber = trim(parseString(m_line2, 2, 5, error));
        if (error != NoError)
        {
            m_lastError = error;
            m_satelliteNumber.clear();
        }
        else
        {
            m_initList.set(Field_SatNumber);
            return m_satelliteNumber;
        }
    }

    if (!m_line3.empty())
    {
        ErrorCode error = NoError;
        m_satelliteNumber = trim(parseString(m_line3, 2, 5, error));
        if (error != NoError)
        {
            m_lastError = error;
            m_satelliteNumber.clear();
        }
        else
        {
            m_initList.set(Field_SatNumber);
        }
    }

    return m_satelliteNumber;
}
//------------------------------------------------------------------------------

void Node::setSatelliteNumber(const std::string &satelliteNumber)
{
    m_satelliteNumber = satelliteNumber;
    m_initList.set(Field_SatNumber);
}
//------------------------------------------------------------------------------

std::string Node::satelliteName() const
{
    if (m_initList.test(Field_SatName) || m_line1.empty())
        return m_satelliteName;

    std::size_t l = m_line1.length();
    if (l > 24)
        l = 24;

    ErrorCode error = NoError;
    m_satelliteName = trim(parseString(m_line1, 0, l, error));
    if (error != NoError)
    {
        m_lastError = error;
        m_satelliteName.clear();
    }
    else
    {
        m_initList.set(Field_SatName);
    }

    return m_satelliteName;
}
//------------------------------------------------------------------------------

void Node::setSatelliteName(const std::string& satelliteName)
{
    m_satelliteName = satelliteName;
    m_initList.set(Field_SatName);
}
//------------------------------------------------------------------------------

std::string Node::designator() const
{
    if (m_initList.test(Field_Designator) || m_line2.empty())
        return m_designator;

    ErrorCode error = NoError;
    m_designator = trim(parseString(m_line2, 9, 8, error));
    if (error != NoError)
    {
        m_lastError = error;
        m_designator.clear();
    }
    else
    {
        m_initList.set(Field_Designator);
    }

    return m_designator;
}
//------------------------------------------------------------------------------

void Node::setDesignator(const std::string &designator)
{
    m_designator = designator;
    m_initList.set(Field_Designator);
}
//------------------------------------------------------------------------------

double Node::n() const
{
    if (m_initList.test(Field_n) || m_line3.empty())
        return m_n;

    ErrorCode error = NoError;
    m_n = parseDouble(m_line3, 52, 11, error) * 2 * M_PI / SECS_IN_DAY;
    if (error != NoError)
    {
        m_lastError = error;
        m_n = 0;
    }
    else
    {
        m_initList.set(Field_n);
    }

    return m_n;
}
//------------------------------------------------------------------------------

void Node::set_n(double n)
{
    m_n = n;
    m_initList.set(Field_n);
}
//------------------------------------------------------------------------------

double Node::dn() const
{
    if (m_initList.test(Field_dn) || m_line2.empty())
        return  m_dn;

    ErrorCode error = NoError;
    m_dn = 2 * parseDouble(m_line2, 33, 10, error)
                                        * 2 * M_PI / SECS_IN_DAY / SECS_IN_DAY;
    if (error != NoError)
    {
        m_lastError = error;
        m_dn = 0;
    }
    else
    {
        m_initList.set(Field_dn);
    }

    return m_dn;
}
//------------------------------------------------------------------------------

void Node::set_dn(double dn)
{
    m_dn = dn;
    m_initList.set(Field_dn);
}
//------------------------------------------------------------------------------

double Node::d2n() const
{
    if (m_initList.test(Field_d2n) || m_line2.empty())
        return m_d2n;

    ErrorCode error = NoError;
    m_d2n = 6 * parseDouble(m_line2, 44, 8, error, true)
                    * 2 * M_PI / SECS_IN_DAY / SECS_IN_DAY / SECS_IN_DAY;
    if (error != NoError)
    {
        m_lastError = error;
        m_d2n = 0;
    }
    else
    {
        m_initList.set(Field_d2n);
    }

    return m_d2n;
}
//------------------------------------------------------------------------------

void Node::set_d2n(double d2n)
{
    m_d2n = d2n;
    m_initList.set(Field_d2n);
}
//------------------------------------------------------------------------------

double Node::i() const
{
    if (m_initList.test(Field_i) || m_line3.empty())
        return m_i;

    ErrorCode error = NoError;
    m_i = deg2rad(parseDouble(m_line3, 8, 8, error));
    if (error != NoError)
    {
        m_lastError = error;
        m_i = 0;
    }

    return m_i;
}
//------------------------------------------------------------------------------

void Node::set_i(double i)
{
    m_i = deg2rad(i);
    m_initList.set(Field_i);
}
double Node::getInclination()
{
	return this->i();
}
void Node::setInclination(double i)
{
	this->set_i(i);
}
//------------------------------------------------------------------------------

double Node::Omega() const
{
    if (m_initList.test(Field_Omega) || m_line3.empty())
        return m_Omega;

    ErrorCode error = NoError;
    m_Omega = deg2rad(parseDouble(m_line3, 17, 8, error));
    if (error != NoError)
    {
        m_lastError = error;
        m_Omega = 0;
    }
    else
    {
        m_initList.set(Field_Omega);
    }

    return m_Omega;
}
void Node::set_Omega(double Omega)
{
    m_Omega = deg2rad(Omega);
    m_initList.set(Field_Omega);
}
double Node::getRightAscensionAscendingNode()
{
	return this->Omega();
}
void Node::setRightAscensionAscendingNode(double raan)
{
	this->set_Omega(raan);
}
//------------------------------------------------------------------------------

double Node::omega() const
{
    if (m_initList.test(Field_omega) || m_line3.empty())
        return m_omega;

    ErrorCode error = NoError;
    m_omega = deg2rad(parseDouble(m_line3, 34, 8, error));
    if (error != NoError)
    {
        m_lastError = error;
        m_omega = 0;
    }
    else
    {
        m_initList.set(Field_omega);
    }

    return m_omega;
}
//------------------------------------------------------------------------------

void Node::set_omega(double omega)
{
    m_omega = deg2rad(omega);
    m_initList.set(Field_omega);
}
//------------------------------------------------------------------------------

double Node::M() const
{
    if (m_initList.test(Field_M) || m_line3.empty())
        return m_M;

    ErrorCode error = NoError;
    m_M = deg2rad(parseDouble(m_line3, 43, 8, error));
    if (error != NoError)
    {
        m_lastError = error;
        m_M = 0;
    }
    else
    {
        m_initList.set(Field_M);
    }

    return m_M;
}
//------------------------------------------------------------------------------

void Node::set_M(double M)
{
    m_M = deg2rad(M);
    m_initList.set(Field_M);
}
//------------------------------------------------------------------------------

double Node::bstar() const
{
    if (m_initList.test(Field_bstar) || m_line2.empty())
        return m_bstar;

    ErrorCode error = NoError;
    m_bstar = parseDouble(m_line2, 53, 8, error, true);
    if (error != NoError)
    {
        m_lastError = error;
        m_bstar = 0;
    }
    else
    {
        m_initList.set(Field_bstar);
    }

    return m_bstar;
}
//------------------------------------------------------------------------------

void Node::set_bstar(double bstar)
{
    m_bstar = bstar;
    m_initList.set(Field_bstar);
}
//------------------------------------------------------------------------------

double Node::e() const
{
    if (m_initList.test(Field_e) || m_line3.empty())
        return m_e;

    ErrorCode error = NoError;
    m_e = parseDouble(m_line3, 26, 8, error, true);
    if (error != NoError)
    {
        m_lastError = error;
        m_e = 0;
    }
    else
    {
        m_initList.set(Field_e);
    }

    return m_e;
}

void Node::set_e(double e)
{
    m_e = e;
    m_initList.set(Field_e);
}

double Node::getEccentricity()
{
	return this->e();
}
void Node::setEccentricity(double e)
{
	this->set_e(e);
}
//------------------------------------------------------------------------------

char Node::classification() const
{
    if (m_initList.test(Field_Classification) || m_line2.empty())
        return m_classification;

    ErrorCode error = NoError;
    m_classification = parseChar(m_line2, 7, error);
    if (error != NoError)
    {
        m_lastError = error;
        m_classification = '\0';
    }
    else
    {
        m_initList.set(Field_Classification);
    }

    return m_classification;
}
//------------------------------------------------------------------------------

void Node::setClassification(char classification)
{
    m_classification = classification;
    m_initList.set(Field_Classification);
}
//------------------------------------------------------------------------------

char Node::ephemerisType() const
{
    if (m_initList.test(Field_EphemerisType) || m_line2.empty())
        return m_ephemerisType;

    ErrorCode error = NoError;
    m_ephemerisType = parseChar(m_line2, 62, error);
    if (error != NoError)
    {
        m_lastError = error;
        m_ephemerisType = '\0';
    }
    else
    {
        m_initList.set(Field_EphemerisType);
    }

    return m_ephemerisType;
}
//------------------------------------------------------------------------------

void Node::setEphemerisType(char ephemerisType)
{
    m_ephemerisType = ephemerisType;
    m_initList.set(Field_EphemerisType);
}
//------------------------------------------------------------------------------

int Node::elementNumber() const
{
    if (m_initList.test(Field_ElementNumber) || m_line2.empty())
        return m_elementNumber;

    ErrorCode error = NoError;
    m_elementNumber = parseInt(m_line2, 64, 4, error);
    if (error != NoError)
    {
        m_lastError = error;
        m_elementNumber = 0;
    }
    else
    {
        m_initList.set(Field_ElementNumber);
    }

    return m_elementNumber;
}
//------------------------------------------------------------------------------

void Node::setElementNumber(int elementNumber)
{
    m_elementNumber = elementNumber;
    m_initList.set(Field_ElementNumber);
}
//------------------------------------------------------------------------------

int Node::revolutionNumber() const
{
    if (m_initList.test(Field_RevolutionNumber) || m_line3.empty())
        return m_revolutionNumber;

    ErrorCode error = NoError;
    m_revolutionNumber = parseInt(m_line3, 63, 5, error);
    if (error != NoError)
    {
        m_lastError = error;
        m_revolutionNumber = 0;
    }
    else
    {
        m_initList.set(Field_RevolutionNumber);
    }

    return m_revolutionNumber;
}
//------------------------------------------------------------------------------

void Node::setRevolutionNumber(int revolutionNumber)
{
    m_revolutionNumber = revolutionNumber;
    m_initList.set(Field_RevolutionNumber);
}
//------------------------------------------------------------------------------

double Node::preciseEpoch() const
{
    if (m_initList.test(Field_date) || m_line2.empty())
        return m_date;

    ErrorCode error = NoError;
    std::string date = parseString(m_line2, 18, 14, error);
    if (error != NoError)
    {
        m_lastError = error;
        m_date = 0;
        return m_date;
    }

    m_date = string2date(date, error);
    if (error != NoError)
    {
        m_lastError = error;
        m_date = 0;
    }
    else
    {
        m_initList.set(Field_date);
    }

    return m_date;
}
//------------------------------------------------------------------------------

void Node::setPreciseEpoch(double preciseEpoch)
{
    m_date = preciseEpoch;
    m_initList.set(Field_date);
}
//------------------------------------------------------------------------------

std::time_t Node::epoch() const
{
    return static_cast<std::time_t>(preciseEpoch());
}
//------------------------------------------------------------------------------

std::string Node::firstString() const
{
    std::string res = satelliteName();
    while (res.length() < 24)
        res += " ";

    return res;
}
//------------------------------------------------------------------------------

std::string Node::secondString() const
{
    std::string res = "1 ";
    res += string2string(satelliteNumber(), 5);
    const char cl = classification();
    res += (isprint(cl) ? std::string(1, cl) : " ") + " ";
    res += string2string(designator(), 8) + " ";
    res += date2string(preciseEpoch(), 14) + " ";
    res += double2string(dn() / 2. * SECS_IN_DAY * SECS_IN_DAY / 2 / M_PI,
                         10, 8, false, false, false) + " ";
    res += double2string(d2n()/6.*SECS_IN_DAY*SECS_IN_DAY*SECS_IN_DAY/2/M_PI,
                         8, 3, true, true, false) + " ";
    res += double2string(bstar(), 8, 3, true, true, false) + " ";
    const char eph = ephemerisType();
    res += (isprint(eph) ? std::string(1, eph) : " ") + " ";
    res += int2string(elementNumber(), 4, false);

    // Checksum
    int sum = checksum(res);
    res += int2string(sum, 1);

    return res;
}
//------------------------------------------------------------------------------

std::string Node::thirdString() const
{
    std::string res = "2 ";
    res += string2string(satelliteNumber(), 5) + " ";
    res += double2string(rad2deg(normalizeAngle(i())), 8, 4, false, false, false) + " ";
    res += double2string(rad2deg(normalizeAngle(Omega())), 8, 4, false, false, false) +
           " ";
    res += double2string(e(), 7, 7, false, true, false) + " ";
    res += double2string(rad2deg(normalizeAngle(omega())), 8, 4, false, false, false) +
           " ";
    res += double2string(rad2deg(normalizeAngle(M())), 8, 4, false, false, false) + " ";
//    res += double2string(n() * SECS_IN_DAY / 2 / M_PI, 11, 8, false, false, false);
    res += double2string(n(), 11, 8, false, false, false);
    res += int2string(revolutionNumber(), 5, false);

    // Checksum
    int sum = checksum(res);
    res += int2string(sum, 1);

    return res;
}
//------------------------------------------------------------------------------

Node& Node::outputFormat(const FileType format)
{
    m_fileType = format;
    return *this;
}
//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    if (node.m_fileType == ThreeLines)
    {
        // Three lines TLE format
        stream << node.firstString() << std::endl;
    }
    stream << node.secondString() << std::endl;
    stream << node.thirdString() << std::endl;

    return stream;
}
//------------------------------------------------------------------------------

double Node::E() const
{
    double E = M();
    double oldE;
    do
    {
        oldE = E;
        E = M() + e() * sin(oldE);
    }
    while (fabs((oldE - E) / E) > E_RELATIVE_ERROR);

    return E;
}
//------------------------------------------------------------------------------

void Node::set_E(double value)
{
    set_M(value - e() * sin(value));
}
//------------------------------------------------------------------------------

double Node::nu() const
{
    return 2 * atan(sqrt( (1 + e()) / (1 - e()) ) * tan(E() / 2) );
}
//------------------------------------------------------------------------------

void Node::set_nu(double value)
{
    set_E(2 * atan(sqrt( (1 - e()) / (1 + e()) ) * tan(value / 2) ));
}
//------------------------------------------------------------------------------

double Node::a() const
{
    return pow(GM / pow(n(), 2), 1./3.);
}
//------------------------------------------------------------------------------

double Node::p() const
{
    return a() * (1 - pow(e(), 2));
}
//------------------------------------------------------------------------------

double Node::r() const
{
    return p() / (1 + e() * cos(nu()));
}
//------------------------------------------------------------------------------

double Node::x() const
{
    double nu = Node::nu();
    return r() * (cos(Omega()) * cos(omega() + nu)
                  - sin(Omega()) * sin(omega() + nu) * cos(i()));
}
//------------------------------------------------------------------------------

double Node::y() const
{
    double nu = Node::nu();
    return r() * (sin(Omega()) * cos(omega() + nu)
                  + cos(Omega()) * sin(omega() + nu) * cos(i()));
}
//------------------------------------------------------------------------------

double Node::z() const
{
    return r() * sin(omega() + nu()) * sin(i());
}
//------------------------------------------------------------------------------

double Node::vx() const
{
    double nu = Node::nu();
    double v0 = sqrt(GM/p());
    double r_dot = v0 * e() * sin(nu);
    double r_nu_dot = v0 * (1 + e() * cos(nu));
    double omega_nu = omega() + nu;

    return (
            (r_dot * cos(omega_nu) - r_nu_dot * sin(omega_nu)) * cos(Omega())
            - (r_dot * sin(omega_nu) + r_nu_dot * cos(omega_nu))
                * sin(Omega()) * cos(i())
    );
}
//------------------------------------------------------------------------------

double Node::vy() const
{
    double nu = Node::nu();
    double v0 = sqrt(GM/p());
    double r_dot = v0 * e() * sin(nu);
    double r_nu_dot = v0 * (1 + e() * cos(nu));
    double omega_nu = omega() + nu;

    return (
            (r_dot * cos(omega_nu) - r_nu_dot * sin(omega_nu)) * sin(Omega())
            + (r_dot * sin(omega_nu) + r_nu_dot * cos(omega_nu))
              * cos(Omega()) * cos(i())
    );
}
//------------------------------------------------------------------------------

double Node::vz() const
{
    double nu = Node::nu();
    double v0 = sqrt(GM/p());
    double r_dot = v0 * e() * sin(nu);
    double r_nu_dot = v0 * (1 + e() * cos(nu));
    double omega_nu = omega() + nu;

    return ( (r_dot * sin(omega_nu) + r_nu_dot * cos(omega_nu)) * sin(i()) );
}
//------------------------------------------------------------------------------

}  // namespace quicktle
