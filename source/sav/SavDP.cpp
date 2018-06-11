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

#include "SavDP.hpp"
#include "PGT.hpp"

SavDP::SavDP(u8* dt)
{
    length = 0x80000;
    boxes = 18;

    data = new u8[length];
    std::copy(dt, dt + length, data);
    GBO();
    SBO();
}

void SavDP::GBO(void)
{
    int ofs = 0xC0F0;
    u8 temp[10];
    u8 dummy[10] = {0xFF};
    
    std::copy(data, data + 10, temp);
    if (!memcmp(temp, dummy, 10)) {
        gbo = 0x40000;
        return;
    }
    
    std::copy(data + 0x40000, data + 0x4000A, temp);
    if (!memcmp(temp, dummy, 10)) {
        gbo = 0;
        return;
    }

    u16 c1 = *(u16*)(data + ofs), c2 = *(u16*)(data + ofs + 0x40000);
    
    gbo = (c1 >= c2) ? 0 : 0x40000;
}

void SavDP::SBO(void)
{
    int ofs = 0x1E2D0;
    u8 temp[10];
    u8 dummy[10] = {0xFF};
    
    std::copy(data, data + 10, temp);
    if (!memcmp(temp, dummy, 10)) {
        sbo = 0x40000;
        return;
    }
    
    std::copy(data + 0x40000, data + 0x4000A, temp);
    if (!memcmp(temp, dummy, 10)) {
        sbo = 0;
        return;
    }

    u16 c1 = *(u16*)(data + ofs), c2 = *(u16*)(data + ofs + 0x40000);
    
    sbo = (c1 >= c2) ? 0 : 0x40000;
}

void SavDP::resign(void)
{
    u8* tmp = new u8[0x12300];
    u16 cs;
    // start, end, chkoffset
    int general[3] = {0x0000, 0xC0EC, 0xC0FE};
    int storage[3] = {0xC100, 0x1E2CC, 0x1E2DE};
    
    std::copy(data + gbo + general[0], data + gbo + general[1] - general[0], tmp);
    cs = ccitt16(tmp, general[1] - general[0]);
    *(u16*)(data + gbo + general[2]) = cs;

    std::copy(data + sbo + storage[0], data + sbo + storage[1] - storage[0], tmp);
    cs = ccitt16(tmp, storage[1] - storage[0]);
    *(u16*)(data + sbo + storage[2]) = cs;
}

u16 SavDP::TID(void) const { return *(u16*)(data + gbo + 0x74); }
void SavDP::TID(u16 v) { *(u16*)(data + gbo + 0x74) = v; }

u16 SavDP::SID(void) const { return *(u16*)(data + gbo + 0x76); }
void SavDP::SID(u16 v) { *(u16*)(data + gbo + 0x76) = v; }

u8 SavDP::version(void) const { return 0; } // Not completely sure how to do this, any ideas?
void SavDP::version(u8 v) { (void)v; }

u8 SavDP::gender(void) const { return data[0x7C + gbo]; }
void SavDP::gender(u8 v) { data[0x7C + gbo] = v; }

u8 SavDP::subRegion(void) const { return 0; } // Todo?
void SavDP::subRegion(u8 v) { (void)v; }

u8 SavDP::country(void) const { return 0; } // Todo?
void SavDP::country(u8 v) { (void)v; }

u8 SavDP::consoleRegion(void) const { return 0; } // Return country?
void SavDP::consoleRegion(u8 v) { (void)v; }

u8 SavDP::language(void) const { return data[0x7D + gbo]; }
void SavDP::language(u8 v) { data[0x7D + gbo] = v; }

std::string SavDP::otName(void) const { return StringUtils::getString4(data, 0x64 + gbo, 8); }
void SavDP::otName(const char* v) { StringUtils::setString4(data, v, 0x64 + gbo, 8); }

u32 SavDP::money(void) const { return *(u32*)(data + gbo + 0x78); }
void SavDP::money(u32 v) { *(u32*)(data + gbo + 0x78) = v; }

u32 SavDP::BP(void) const { return *(u16*)(data + gbo + 0x84); } // Returns Coins @ Game Corner
void SavDP::BP(u32 v) { *(u16*)(data + gbo + 0x84) = v; }

u16 SavDP::playedHours(void) const { return *(u16*)(data + gbo + 0x86); }
void SavDP::playedHours(u16 v) { *(u16*)(data + gbo + 0x86) = v; }

u8 SavDP::playedMinutes(void) const { return data[gbo + 0x88]; }
void SavDP::playedMinutes(u8 v) { data[gbo + 0x88] = v; }

u8 SavDP::playedSeconds(void) const { return data[gbo + 0x89]; }
void SavDP::playedSeconds(u8 v) { data[gbo + 0x89] = v; }

u8 SavDP::currentBox(void) const { return data[0xC100]; }
void SavDP::currentBox(u8 v) { data[0xC100] = v; }

u32 SavDP::boxOffset(u8 box, u8 slot) const { return sbo + 0xC104 + 136*box*30 + slot*136; }
u32 SavDP::partyOffset(u8 slot) const { return gbo + 0x98 + slot*236; }

std::unique_ptr<PKX> SavDP::pkm(u8 slot) const
{
    u8 buf[136];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 136, buf);
    return std::unique_ptr<PKX>(new PK4(buf, true));
}
std::unique_ptr<PKX> SavDP::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 buf[136];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 136, buf);
    return std::unique_ptr<PKX>(new PK4(buf, ekx));
}

void SavDP::pkm(PKX& pk, u8 box, u8 slot)
{
    PK4* pk4 = (PK4*)&pk;
    std::copy(pk4->data, pk4->data + 136, data + boxOffset(box, slot));
}

void SavDP::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < boxes; box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::unique_ptr<PKX> pk4 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pk4->encrypt();
            }
            pkm(*pk4, box, slot);
        }
    }
}

void SavDP::mysteryGift(WCX& wc, int& pos)
{
    PGT* pgt = (PGT*)&wc;
    *(data + 0xB4C0 + gbo + (2047 >> 3)) = 0x80;
    std::copy(pgt->data, pgt->data + 260, data + 0xB5C0 + gbo + pos);
    pos++;
}

std::string SavDP::boxName(u8 box) const { return StringUtils::getString4(data, boxOffset(18, 0) + box*0x28, 0x14); }

u8 SavDP::partyCount(void) const { return data[gbo + 0xA0]; }

void SavDP::dex(PKX& pk)
{
    if (pk.species() == 0 || pk.species() > 493)
    {
        return;
    }

    static const int brSize = 0x40;
    int bit = pk.species() - 1;
    u8 mask = (u8)(1 << (bit & 7));
    int ofs = gbo + 0x12DC + (bit >> 3) + 0x4;

    /* 4 BitRegions with 0x40*8 bits
    * Region 0: Caught (Captured/Owned) flags
    * Region 1: Seen flags
    * Region 2/3: Toggle for gender display
    * 4 possible states: 00, 01, 10, 11
    * 00 - 1Seen: Male Only
    * 01 - 2Seen: Male First, Female Second
    * 10 - 2Seen: Female First, Male Second
    * 11 - 1Seen: Female Only
    * (bit1 ^ bit2) + 1 = forms in dex
    * bit2 = male/female shown first toggle */

    // Set the species() Owned Flag
    data[ofs + brSize * 0] |= mask;

    // Check if already Seen
    if ((data[ofs + brSize * 1] & mask) == 0) // Not seen
    {
        data[ofs + brSize * 1] |= mask; // Set seen
        int gr = PersonalDPPtHGSS::gender(pk.species());
        switch (gr)
        {
            case 255: // Genderless
            case 0: // Male Only
                data[ofs + brSize * 2] &= mask;
                data[ofs + brSize * 3] &= mask;
                break;
            case 254: // Female Only
                data[ofs + brSize * 2] |= mask;
                data[ofs + brSize * 3] |= mask;
                break;
            default: // Male or Female
                bool m = (data[ofs + brSize * 2] & mask) != 0;
                bool f = (data[ofs + brSize * 3] & mask) != 0;
                if (m || f) // bit already set?
                    break;
                int gender = pk.gender() & 1;
                data[ofs + brSize * 2] &= (u8)~mask; // unset
                data[ofs + brSize * 3] &= (u8)~mask; // unset
                gender ^= 1; // Set OTHER gender seen bit so it appears second
                data[ofs + brSize * (2 + gender)] |= mask;
                break;
        }
    }

    int formOffset = gbo + 0x12DC + 4 + brSize * 4 + 4;
    
    u8 forms[0x1C];
    bool validFormArray = getForms(forms, pk.species());
    int fc = PersonalDPPtHGSS::formCount(pk.species());
    if (validFormArray && fc > 0)
    {
        if (pk.species() == 201) // Unown
        {
            for (u8 i = 0; i < 0x1C; i++)
            {
                u8 val = data[formOffset + 4 + i];
                if (val == pk.alternativeForm())
                    break; // already set
                if (val != 0xFF)
                    continue; // keep searching

                data[formOffset + 4 + i] = (u8)pk.alternativeForm();
                break; // form now set
            }
        }
        else
        {
            checkInsertForm(forms, fc, pk.alternativeForm());
            setForms(forms, pk.species());
        }
    }

    int DPLangSpecies[] = { 23, 25, 54, 77, 120, 129, 202, 214, 215, 216, 228, 278, 287, 315 };
    int dpl = 0;
    for (int i = 0; i < 14; i++)
    {
        if (pk.species() == DPLangSpecies[i])
        {
            dpl += i + 1;
            break;
        }
    }
    if (dpl == 0) return;

    // Set the Language
    int PokeDexLanguageFlags = formOffset + 0x20;
    int lang = pk.language() - 1;
    switch (lang) // invert ITA/GER
    {
        case 3:
            lang = 4;
            break;
        case 4:
            lang = 3;
            break;
    }
    if (lang > 5)
        lang = 0; // no KOR+
    lang = (lang < 0) ? 1 : lang; // default English
    data[PokeDexLanguageFlags + dpl] |= (u8)(1 << lang);
}

bool SavDP::checkInsertForm(u8* forms, u8 fc, u8 formNum)
{
    for (u8 i = 0; i < fc; i++)
    {
        if (forms[i] == formNum)
        {
            return false;
        }
    }
    
    u8 dummy[fc] = {255};
    if (!memcmp(forms, dummy, fc))
    {
        forms[0] = formNum;
        return true;
    }
    
    // insert at first empty
    u8 index = 255;
    for (int i = 0; i < fc; i++)
    {
        if (forms[i] == 255)
        {
            index = i;
        }
    }
   
    if (index == 255)
    {
        return false;        
    }

    forms[index] = formNum;
    return true;
}

bool SavDP::getForms(u8* forms, u16 species)
{
    static const int brSize = 0x40;
    if (species == 386)
    {
        u32 val = (u32)data[gbo + 0x12DC + 0x4 + 1*brSize - 1] | (u32)data[gbo + 0x12DC + 0x4 + 2*brSize - 1] << 8;
        getDexFormValues(forms, val, 4, 4);
        return true;
    }

    int formOffset = gbo + 0x12DC + 4 + 4*brSize + 4;
    switch (species)
    {
        case 422: // Shellos
            getDexFormValues(forms, data[formOffset + 0], 1, 2);
            return true;
        case 423: // Gastrodon
            getDexFormValues(forms, data[formOffset + 1], 1, 2);
            return true;
        case 412: // Burmy
            getDexFormValues(forms, data[formOffset + 2], 2, 3);
            return true;
        case 413: // Wormadam
            getDexFormValues(forms, data[formOffset + 3], 2, 3);
            return true;
        case 201:
            int ofs = formOffset + 4;
            std::copy(data + ofs, data + ofs + 0x1C, forms);
            return true;
    }

    return false;
}

void SavDP::getDexFormValues(u8* forms, u32 v, u8 bitsPerForm, u8 readCt)
{
    int n1 = 0xFF >> (8 - bitsPerForm);
    for (int i = 0; i < readCt; i++)
    {
        int val = (int)(v >> (i * bitsPerForm)) & n1;
        forms[i] = n1 == val && bitsPerForm > 1 ? 255 : val;
    }

    if (bitsPerForm == 1 && forms[0] == forms[1] && forms[0] == 1)
    {
        forms[0] = forms[1] = 255;
    }
}

void SavDP::setForms(u8* forms, u16 species)
{
    static const int brSize = 0x40;
    switch (species)
    {
        case 386: // Deoxys
            u32 newval = setDexFormValues(forms, 4, 4);
            data[gbo + 0x12DC + 0x4 + 1*brSize - 1] = (u8) (newval & 0xFF);
            data[gbo + 0x12DC + 0x4 + 2*brSize - 1] = (u8) ((newval >> 8) & 0xFF);
            break;
    }

    int formOffset = gbo + 0x12DC + 4 + 4*brSize + 4;
    switch (species)
    {
        case 422: // Shellos
            data[formOffset + 0] = (u8)setDexFormValues(forms, 2, 1);
            return;
        case 423: // Gastrodon
            data[formOffset + 1] = (u8)setDexFormValues(forms, 2, 1);
            return;
        case 412: // Burmy
            data[formOffset + 2] = (u8)setDexFormValues(forms, 3, 2);
            return;
        case 413: // Wormadam
            data[formOffset + 3] = (u8)setDexFormValues(forms, 3, 2);
            return;
        case 201: // Unown
        {
            int ofs = formOffset + 4;
            // TODO: missing 0xFF handling
            std::copy(forms, forms + 0x1C, data + ofs);
            return;
        }
    }
}

u32 SavDP::setDexFormValues(u8* forms, u8 fc, u8 bitsPerForm)
{
    int n1 = 0xFF >> (8 - bitsPerForm);
    u32 v = 0xFFFFFFFF << (fc*bitsPerForm);
    for (u8 i = 0; i < fc; i++)
    {
        u8 val = forms[i] == 255 ? n1 : forms[i];
        v |= (u32)(val << (bitsPerForm*i));
    }
    return v;
}