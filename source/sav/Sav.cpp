/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "Sav.hpp"
#include "SavB2W2.hpp"
#include "SavBW.hpp"
#include "SavDP.hpp"
#include "SavHGSS.hpp"
#include "SavORAS.hpp"
#include "SavPT.hpp"
#include "SavSUMO.hpp"
#include "SavUSUM.hpp"
#include "SavXY.hpp"

const u16 Sav::crc16[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

Sav::~Sav() { delete[] data; }

u16 Sav::ccitt16(const u8* buf, u32 len)
{
    u16 crc = 0xFFFF;
    for (u32 i = 0; i < len; i++)
    {
        crc ^= (u16)(buf[i] << 8);
        for (u32 j = 0; j < 0x8; j++)
        {
            if ((crc & 0x8000) > 0)
                crc = (u16)((crc << 1) ^ 0x1021);
            else
                crc <<= 1;
        }
    }
    return crc;
}

std::unique_ptr<Sav> Sav::getSave(u8* dt, size_t length)
{
    switch (length)
    {
        case 0x6CC00:
            return std::unique_ptr<Sav>(new SavUSUM(dt));
        case 0x6BE00:
            return std::unique_ptr<Sav>(new SavSUMO(dt));
        case 0x76000:
            return std::unique_ptr<Sav>(new SavORAS(dt));
        case 0x65600:
            return std::unique_ptr<Sav>(new SavXY(dt));
        case 0x80000:
            return checkDSType(dt);
        default:
            return std::unique_ptr<Sav>(nullptr);
    }
}

bool Sav::isValidDSSave(u8* dt)
{
    u16 chk1 = *(u16*)(dt + 0x24000 - 0x100 + 0x8C + 0xE);
    u16 actual1 = ccitt16(dt + 0x24000 - 0x100, 0x8C);
    if (chk1 == actual1)
    {
        return true;
    }
    u16 chk2 = *(u16*)(dt + 0x26000 - 0x100 + 0x94 + 0xE);
    u16 actual2 = ccitt16(dt + 0x26000 - 0x100, 0x94);
    if (chk2 == actual2)
    {
        return true;
    }

    if (*(u16*)(dt + 0xC0FE) == ccitt16(dt, 0xC0EC))
        return true;
    if (*(u16*)(dt + 0xCF2A) == ccitt16(dt, 0xCF18))
        return true;
    if (*(u16*)(dt + 0xF626) == ccitt16(dt, 0xF618))
        return true;

    // General Block Checksum is invalid, check for block identifiers
    u8 dpPattern[] = { 0x00, 0xC1, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 ptPattern[] = { 0x2C, 0xCF, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 hgssPattern[] = { 0x28, 0xF6, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    if (validSequence(dt, dpPattern))
        return true;
    if (validSequence(dt, ptPattern))
        return true;
    if (validSequence(dt, hgssPattern))
        return true;

    // Check the other save
    if (validSequence(dt, dpPattern, 0x40000))
        return true;
    if (validSequence(dt, ptPattern, 0x40000))
        return true;
    if (validSequence(dt, hgssPattern, 0x40000))
        return true;
    return false;
}

std::unique_ptr<Sav> Sav::checkDSType(u8* dt)
{
    u16 chk1 = *(u16*)(dt + 0x24000 - 0x100 + 0x8C + 0xE);
    u16 actual1 = ccitt16(dt + 0x24000 - 0x100, 0x8C);
    if (chk1 == actual1)
    {
        return std::unique_ptr<Sav>(new SavBW(dt));
    }
    u16 chk2 = *(u16*)(dt + 0x26000 - 0x100 + 0x94 + 0xE);
    u16 actual2 = ccitt16(dt + 0x26000 - 0x100, 0x94);
    if (chk2 == actual2)
    {
        return std::unique_ptr<Sav>(new SavB2W2(dt));
    }

    if (*(u16*)(dt + 0xC0FE) == ccitt16(dt, 0xC0EC))
        return std::unique_ptr<Sav>(new SavDP(dt));
    if (*(u16*)(dt + 0xCF2A) == ccitt16(dt, 0xCF18))
        return std::unique_ptr<Sav>(new SavPT(dt));
    if (*(u16*)(dt + 0xF626) == ccitt16(dt, 0xF618))
        return std::unique_ptr<Sav>(new SavHGSS(dt));

    // General Block Checksum is invalid, check for block identifiers
    u8 dpPattern[] = { 0x00, 0xC1, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 ptPattern[] = { 0x2C, 0xCF, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 hgssPattern[] = { 0x28, 0xF6, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    if (validSequence(dt, dpPattern))
        return std::unique_ptr<Sav>(new SavDP(dt));
    if (validSequence(dt, ptPattern))
        return std::unique_ptr<Sav>(new SavPT(dt));
    if (validSequence(dt, hgssPattern))
        return std::unique_ptr<Sav>(new SavHGSS(dt));

    // Check the other save
    if (validSequence(dt, dpPattern, 0x40000))
        return std::unique_ptr<Sav>(new SavDP(dt));
    if (validSequence(dt, ptPattern, 0x40000))
        return std::unique_ptr<Sav>(new SavPT(dt));
    if (validSequence(dt, hgssPattern, 0x40000))
        return std::unique_ptr<Sav>(new SavHGSS(dt));
    return std::unique_ptr<Sav>(nullptr);
}

bool Sav::validSequence(u8* dt, u8* pattern, int shift)
{
    int ofs = *(u16*)(pattern) - 0xC + shift;
    for (int i = 0; i < 10; i++)
        if (dt[i + ofs] != pattern[i])
            return false;
    return true;
}