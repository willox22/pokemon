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

#include "PK5.hpp"

void PK5::shuffleArray(void)
{
    static const int blockLength = 32;
    u8 seed = (((encryptionConstant() & 0x3E000) >> 0xD) % 24);
    static int aloc[24] = { 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 };
    static int bloc[24] = { 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 };
    static int cloc[24] = { 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1 };
    static int dloc[24] = { 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0 };
    int ord[4] = {aloc[seed], bloc[seed], cloc[seed], dloc[seed]};

    u8 cdata[length];
    std::copy(data, data + length, cdata);

    for (u8 i = 0; i < 4; i++)
    {
        std::copy(
            cdata + blockLength * ord[i] + 8, 
            cdata + blockLength * ord[i] + blockLength + 8, 
            data + 8 + blockLength * i
        );
    }
}

void PK5::crypt(void)
{
    u32 chk = checksum();

    for (int i = 0x08; i < 4 * 32 + 8; i += 2)
    {
        chk = seedStep(chk);
        data[i] ^= (chk >> 16);
        data[i+1] ^= (chk >> 24);
    }
}

PK5::PK5(u8* dt, bool ekx)
{
    length = 136;

    std::copy(dt, dt + length, data);
    if (ekx)
        decrypt();
}

void PK5::decrypt(void)
{
    crypt();
    shuffleArray();
}

void PK5::encrypt(void)
{
    refreshChecksum();
    for (int i = 0; i < 11; i++)
    {
        shuffleArray();
    }
    crypt();
}

std::unique_ptr<PKX> PK5::clone(void) { return std::unique_ptr<PKX>(new PK5(data)); }

u8 PK5::generation(void) const { return 5; }

u32 PK5::encryptionConstant(void) const { return PID(); }
void PK5::encryptionConstant(u32 v) { (void)v; }

u8 PK5::currentFriendship(void) const { return otFriendship(); }
void PK5::currentFriendship(u8 v) { otFriendship(v); }

u8 PK5::currentHandler(void) const { return 0; }
void PK5::currentHandler(u8 v) { (void)v; }

u8 PK5::abilityNumber(void) const { return hiddenAbility() ? 4 : 1 << ((PID() >> 16) & 1); }
void PK5::abilityNumber(u8 v) { (void)v; }

u32 PK5::PID(void) const { return *(u32*)(data); }
void PK5::PID(u32 v) { *(u32*)(data) = v; }

u16 PK5::sanity(void) const { return *(u16*)(data + 0x04); }
void PK5::sanity(u16 v) { *(u16*)(data + 0x04) = v; }

u16 PK5::checksum(void) const { return *(u16*)(data + 0x06); }
void PK5::checksum(u16 v) { *(u16*)(data + 0x06) = v; }

u16 PK5::species(void) const { return *(u16*)(data + 0x08); }
void PK5::species(u16 v) { *(u16*)(data + 0x08) = v; }

u16 PK5::heldItem(void) const { return *(u16*)(data + 0x0A); }
void PK5::heldItem(u16 v) { *(u16*)(data + 0x0A) = v; }

u16 PK5::TID(void) const { return *(u16*)(data + 0x0C); }
void PK5::TID(u16 v) { *(u16*)(data + 0x0C) = v; }

u16 PK5::SID(void) const { return *(u16*)(data + 0x0E); }
void PK5::SID(u16 v) { *(u16*)(data + 0x0E) = v; }

u32 PK5::experience(void) const { return *(u32*)(data + 0x10); }
void PK5::experience(u32 v) { *(u32*)(data + 0x10) = v; }

u8 PK5::otFriendship(void) const { return data[0x14]; }
void PK5::otFriendship(u8 v) { data[0x14] = v; }

u8 PK5::ability(void) const { return data[0x15]; }
void PK5::ability(u8 v) { data[0x15] = v; }

u16 PK5::markValue(void) const { return data[0x16]; }
void PK5::markValue(u16 v) { data[0x16] = v; }

u8 PK5::language(void) const { return data[0x17]; }
void PK5::language(u8 v) { data[0x17] = v; }

u8 PK5::ev(u8 ev) const { return data[0x18 + ev]; }
void PK5::ev(u8 ev, u8 v) { data[0x18 + ev] = v; }

u8 PK5::contest(u8 contest) const { return data[0x1E + contest]; }
void PK5::contest(u8 contest, u8 v) { data[0x1E + contest] = v; }

bool PK5::ribbon(u8 ribcat, u8 ribnum) const
{
    static u8 ribIndex[12] = { 0x24, 0x25, 0x26, 0x27, 0x3C, 0x3D, 0x3E, 0x3F, 0x60, 0x61, 0x62, 0x63 };
    return (data[ribIndex[ribcat]] & (1 << ribnum)) == 1 << ribnum;
}

void PK5::ribbon(u8 ribcat, u8 ribnum, u8 v)
{
    static u8 ribIndex[12] = { 0x24, 0x25, 0x26, 0x27, 0x3C, 0x3D, 0x3E, 0x3F, 0x60, 0x61, 0x62, 0x63 };
    data[ribIndex[ribcat]] = (u8)((data[ribIndex[ribcat]] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0));
}

u16 PK5::move(u8 m) const { return *(u16*)(data + 0x28 + m*2); }
void PK5::move(u8 m, u16 v) { *(u16*)(data + 0x28 + m*2) = v; }

u8 PK5::PP(u8 m) const { return data[0x30 + m]; }
void PK5::PP(u8 m, u8 v) { data[0x30 + m] = v; }

u8 PK5::PPUp(u8 m) const { return data[0x34 + m]; }
void PK5::PPUp(u8 m, u8 v) { data[0x34 + m] = v; }

u8 PK5::iv(u8 stat) const
{
    u32 buffer = *(u32*)(data + 0x38);
    return (u8)((buffer >> 5*stat) & 0x1F);
}

void PK5::iv(u8 stat, u8 v)
{
    u32 buffer = *(u32*)(data + 0x38);
    u32 mask = 0xFFFFFFFF ^ 0x1F << 5*stat;
    buffer &= mask;
    buffer ^= ((v & 0x1F) << (5*stat));
    *(u32*)(data + 0x38) = buffer;
}

bool PK5::egg(void) const { return ((*(u32*)(data + 0x38) >> 30) & 0x1) == 1; }
void PK5::egg(bool v) { *(u32*)(data + 0x38) = (u32)((*(u32*)(data + 0x38) & ~0x40000000) | (u32)(v ? 0x40000000 : 0)); }

bool PK5::nicknamed(void) const { return ((*(u32*)(data + 0x38) >> 31) & 0x1) == 1; }
void PK5::nicknamed(bool v) { *(u32*)(data + 0x38) = (*(u32*)(data + 0x38) & 0x7FFFFFFF) | (v ? 0x80000000 : 0); }

bool PK5::fatefulEncounter(void) const { return (data[0x40] & 1) == 1; }
void PK5::fatefulEncounter(bool v) { data[0x40] = (u8)((data[0x40] & ~0x01) | (v ? 1 : 0)); }

u8 PK5::gender(void) const { return (data[0x40] >> 1) & 0x3; }
void PK5::gender(u8 g) { data[0x40] = u8((data[0x40] & ~0x06) | (g << 1)); }

u8 PK5::alternativeForm(void) const { return data[0x40] >> 3; }
void PK5::alternativeForm(u8 v) { data[0x40] = u8((data[0x40] & 0x07) | (v << 3)); }

u8 PK5::nature(void) const { return data[0x41]; }
void PK5::nature(u8 v) { data[0x41] = v; }

bool PK5::hiddenAbility(void) const { return (data[0x42] & 1) == 1; }
void PK5::hiddenAbility(bool v) { data[0x42] = (u8)((data[0x42] & ~0x01) | (v ? 1 : 0)); }

bool PK5::nPokemon(void) const { return (data[0x42] & 2) == 2; }
void PK5::nPokemon(bool v) { data[0x42] = (u8)((data[0x42] & ~0x02) | (v ? 2 : 0)); }

std::string PK5::nickname(void) const { return StringUtils::getTrimmedString(data, 0x48, 11, (char*)"\uFFFF"); }
void PK5::nickname(const char* v) { StringUtils::setStringWithBytes(data, v, 0x48, 11, (char*)"\uFFFF"); }

u8 PK5::version(void) const { return data[0x5F]; }
void PK5::version(u8 v) { data[0x5F] = v; }

std::string PK5::otName(void) const { return StringUtils::getTrimmedString(data, 0x68, 8, (char*)"\uFFFF"); }
void PK5::otName(const char* v) { StringUtils::setStringWithBytes(data, v, 0x68, 8, (char*)"\uFFFF"); }

u8 PK5::eggYear(void) const { return data[0x78]; }
void PK5::eggYear(u8 v) { data[0x78] = v; }

u8 PK5::eggMonth(void) const { return data[0x79]; }
void PK5::eggMonth(u8 v) { data[0x79] = v;}

u8 PK5::eggDay(void) const { return data[0x7A]; }
void PK5::eggDay(u8 v) { data[0x7A] = v; }

u8 PK5::metYear(void) const { return data[0x7B]; }
void PK5::metYear(u8 v) { data[0x7B] = v; }

u8 PK5::metMonth(void) const { return data[0x7C]; }
void PK5::metMonth(u8 v) { data[0x7C] = v; }

u8 PK5::metDay(void) const { return data[0x7D]; }
void PK5::metDay(u8 v) { data[0x7D] = v; }

u16 PK5::eggLocation(void) const { return *(u16*)(data + 0x78); }
void PK5::eggLocation(u16 v) { *(u16*)(data + 0x78) = v; }

u16 PK5::metLocation(void) const { return *(u16*)(data + 0x80); }
void PK5::metLocation(u16 v) { *(u16*)(data + 0x80) = v; }

u8 PK5::pkrs(void) const { return data[0x82]; }
void PK5::pkrs(u8 v) { data[0x82] = v; }

u8 PK5::pkrsDays(void) const { return data[0x82] & 0xF; };
void PK5::pkrsDays(u8 v) { data[0x82] = (u8)((data[0x82] & ~0xF) | v); }

u8 PK5::pkrsStrain(void) const { return data[0x82] >> 4; };
void PK5::pkrsStrain(u8 v) { data[0x82] = (u8)((data[0x82] & 0xF) | v << 4); }

u8 PK5::ball(void) const { return data[0x83]; }
void PK5::ball(u8 v) { data[0x83] = v; }

u8 PK5::metLevel(void) const { return data[0x84] & ~0x80; }
void PK5::metLevel(u8 v) { data[0x84] = (data[0x84] & 0x80) | v; }

u8 PK5::otGender(void) const { return data[0x84] >> 7; }
void PK5::otGender(u8 v) { data[0x84] = (data[0x84] & ~0x80) | (v << 7); }

u8 PK5::encounterType(void) const { return data[0x85]; }
void PK5::encounterType(u8 v) { data[0x85] = v; }

void PK5::refreshChecksum(void)
{
    u16 chk = 0;
    for (u8 i = 8; i < length; i += 2)
    {
        chk += *(u16*)(data + i);
    }
    checksum(chk);
}

u8 PK5::hpType(void) const { return 15 * ((iv(0) & 1) + 2*(iv(1) & 1) + 4*(iv(2) & 1) + 8*(iv(3) & 1) + 16*(iv(4) & 1) + 32*(iv(5) & 1)) / 63; }
void PK5::hpType(u8 v)
{
    static const u8 hpivs[16][6] = {
        { 1, 1, 0, 0, 0, 0 }, // Fighting
        { 0, 0, 0, 1, 0, 0 }, // Flying
        { 1, 1, 0, 1, 0, 0 }, // Poison
        { 1, 1, 1, 1, 0, 0 }, // Ground
        { 1, 1, 0, 0, 1, 0 }, // Rock
        { 1, 0, 0, 1, 1, 0 }, // Bug
        { 1, 0, 1, 1, 1, 0 }, // Ghost
        { 1, 1, 1, 1, 1, 0 }, // Steel
        { 1, 0, 1, 0, 0, 1 }, // Fire
        { 1, 0, 0, 1, 0, 1 }, // Water
        { 1, 0, 1, 1, 0, 1 }, // Grass
        { 1, 1, 1, 1, 0, 1 }, // Electric
        { 1, 0, 1, 0, 1, 1 }, // Psychic
        { 1, 0, 0, 1, 1, 1 }, // Ice
        { 1, 0, 1, 1, 1, 1 }, // Dragon
        { 1, 1, 1, 1, 1, 1 }, // Dark
    };

    for (u8 i = 0; i < 6; i++)
    {
        iv((iv(i) & 0x1E) + hpivs[v][i], i);
    }
}

u16 PK5::TSV(void) const { return (TID() ^ SID()) >> 3; }
u16 PK5::PSV(void) const { return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 3; }

u8 PK5::level(void) const
{
    u8 i = 1;
    u8 xpType = PersonalBWB2W2::expType(species());
    while (i++ < 100 && experience() >= expTable(i, xpType));
    return i;
}

void PK5::level(u8 v)
{
    experience(expTable(v - 1, PersonalBWB2W2::expType(species())));
}

bool PK5::shiny(void) const { return TSV() == PSV(); }
void PK5::shiny(bool v)
{
    if (v)
    {
        u16 buf = (PID() >> 16) ^ (TSV() << 4);
        *(u16*)(data + 0x18) = buf;
    }
    else
    {
        srand(PID());
        PID(rand());
    }
}

u16 PK5::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form = alternativeForm();
    u8 formcount = PersonalBWB2W2::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies = PersonalBWB2W2::formStatIndex(tmpSpecies);
        if (!tmpSpecies)
        {
            tmpSpecies = backSpecies;
        }
        else if (form < formcount)
        {
            tmpSpecies += form - 1;
        }
    }

    return tmpSpecies;
}

u16 PK5::stat(const u8 stat) const
{
    u16 tmpSpecies = formSpecies(), final;
    u8 mult = 10, basestat = 0;

    if (stat == 0) basestat = PersonalBWB2W2::baseHP(tmpSpecies);
    else if (stat == 1) basestat = PersonalBWB2W2::baseAtk(tmpSpecies);
    else if (stat == 2) basestat = PersonalBWB2W2::baseDef(tmpSpecies);
    else if (stat == 3) basestat = PersonalBWB2W2::baseSpe(tmpSpecies);
    else if (stat == 4) basestat = PersonalBWB2W2::baseSpa(tmpSpecies);
    else if (stat == 5) basestat = PersonalBWB2W2::baseSpd(tmpSpecies);

    if (stat == 0) 
        final = 10 + (2 * basestat + iv(stat) + ev(stat) / 4 + 100) * level() / 100;
    else
        final = 5 + (2 * basestat + iv(stat) + ev(stat) / 4) * level() / 100; 
    if (nature() / 5 + 1 == stat) mult++;
    if (nature() % 5 + 1 == stat) mult--;
    return final * mult / 10;
}

std::unique_ptr<PKX> PK5::next(void)
{
    u8 dt[232] = {0};
    PK6 *pk6 = new PK6(dt);

    pk6->encryptionConstant(PID());
    pk6->species(species());
    pk6->TID(TID());
    pk6->SID(SID());
    pk6->experience(experience());
    pk6->PID(PID());
    pk6->ability(ability());

    u8 abilities[3] = { PersonalBWB2W2::ability(species(), 0), PersonalBWB2W2::ability(species(), 1), PersonalBWB2W2::ability(species(), 2) };
    u8 abilVal = std::distance(abilities, std::find(abilities, abilities + 3, ability()));
    if (abilVal <= 3 && abilities[abilVal] == abilities[2] && hiddenAbility())
    {
        abilVal = 2; // HA shared by normal ability
    }
    if (abilVal <= 3)
    {
        pk6->abilityNumber(1 << abilVal);
    }
    else // Shouldn't happen
    {
        if (hiddenAbility())
        {
            pk6->abilityNumber(4);
        }
        else
        {
            pk6->abilityNumber(gen5() ? ((PID() >> 16) & 1) : 1 << (PID() & 1));
        }
    }

    pk6->markValue(markValue());
    pk6->language(language());
    
    for (int i = 0; i < 6; i++)
    {
        // EV Cap
        pk6->ev(i, ev(i) > 252 ? 252 : ev(i));
        pk6->iv(i, iv(i));
        pk6->contest(i, contest(i));
    }

    for (int i = 0; i < 4; i++)
    {
        pk6->move(i, move(i));
        pk6->PPUp(i, PPUp(i));
        pk6->PP(i, PP(i));
    }

    pk6->egg(egg());
    pk6->nicknamed(nicknamed());

    pk6->fatefulEncounter(fatefulEncounter());
    pk6->gender(gender());
    pk6->alternativeForm(alternativeForm());
    pk6->nature(nature());

    pk6->nickname(i18n::species(pk6->language(), pk6->species()).c_str());
    if (nicknamed())
        pk6->nickname(nickname().c_str());

    pk6->version(version());

    pk6->otName(otName().c_str());

    pk6->metYear(metYear());
    pk6->metMonth(metMonth());
    pk6->metDay(metDay());
    pk6->eggYear(eggYear());
    pk6->eggMonth(eggMonth());
    pk6->eggDay(eggDay());

    pk6->metLocation(metLocation());
    pk6->eggLocation(eggLocation());

    pk6->pkrsStrain(pkrsStrain());
    pk6->pkrsDays(pkrsDays());
    pk6->ball(ball());

    pk6->metLevel(metLevel());
    pk6->otGender(otGender());
    pk6->encounterType(encounterType());

    // Ribbon
    u8 contestRibbon = 0;
    u8 battleRibbon = 0;

    for (int i = 0; i < 8; i++) // Sinnoh 3, Hoenn 1
    {
        if (((data[0x60] >> i) & 1) == 1) contestRibbon++;
        if (((data[0x61] >> i) & 1) == 1) contestRibbon++;
        if (((data[0x3C] >> i) & 1) == 1) contestRibbon++;
        if (((data[0x3D] >> i) & 1) == 1) contestRibbon++;
    }
    for (int i = 0; i < 4; i++) // Sinnoh 4, Hoenn 2
    {
        if (((data[0x62] >> i) & 1) == 1) contestRibbon++;
        if (((data[0x3E] >> i) & 1) == 1) contestRibbon++;
    }

    // Winning Ribbon
    if (((data[0x3E] & 0x20) >> 5) == 1) battleRibbon++;
    // Victory Ribbon
    if (((data[0x3E] & 0x40) >> 6) == 1) battleRibbon++;
    for (int i = 1; i < 7; i++) // Sinnoh Battle Ribbons
        if (((data[0x24] >> i) & 1) == 1) battleRibbon++;

    pk6->ribbonContestCount(contestRibbon);
    pk6->ribbonBattleCount(battleRibbon);

    pk6->ribbon(0, 1, ribbon(6, 4)); // Hoenn Champion
    pk6->ribbon(0, 2, ribbon(0, 0)); // Sinnoh Champ
    pk6->ribbon(0, 7, ribbon(7, 0)); // Effort Ribbon

    pk6->ribbon(1, 0, ribbon(0, 7)); // Alert
    pk6->ribbon(1, 1, ribbon(1, 0)); // Shock
    pk6->ribbon(1, 2, ribbon(1, 1)); // Downcast
    pk6->ribbon(1, 3, ribbon(1, 2)); // Careless
    pk6->ribbon(1, 4, ribbon(1, 3)); // Relax
    pk6->ribbon(1, 5, ribbon(1, 4)); // Snooze
    pk6->ribbon(1, 6, ribbon(1, 5)); // Smile
    pk6->ribbon(1, 7, ribbon(1, 6)); // Gorgeous

    pk6->ribbon(2, 0, ribbon(1, 7)); // Royal
    pk6->ribbon(2, 1, ribbon(2, 0)); // Gorgeous Royal
    pk6->ribbon(2, 2, ribbon(6, 7)); // Artist
    pk6->ribbon(2, 3, ribbon(2, 1)); // Footprint
    pk6->ribbon(2, 4, ribbon(2, 2)); // Record
    pk6->ribbon(2, 5, ribbon(2, 4)); // Legend
    pk6->ribbon(2, 6, ribbon(7, 4)); // Country
    pk6->ribbon(2, 7, ribbon(7, 5)); // National

    pk6->ribbon(3, 0, ribbon(7, 6)); // Earth
    pk6->ribbon(3, 1, ribbon(7, 7)); // World
    pk6->ribbon(3, 2, ribbon(3, 2)); // Classic
    pk6->ribbon(3, 3, ribbon(3, 3)); // Premier
    pk6->ribbon(3, 4, ribbon(2, 3)); // Event
    pk6->ribbon(3, 5, ribbon(2, 6)); // Birthday
    pk6->ribbon(3, 6, ribbon(2, 7)); // Special
    pk6->ribbon(3, 7, ribbon(3, 0)); // Souvenir

    pk6->ribbon(4, 0, ribbon(3, 1)); // Wishing Ribbon
    pk6->ribbon(4, 1, ribbon(7, 1)); // Battle Champion
    pk6->ribbon(4, 2, ribbon(7, 2)); // Regional Champion
    pk6->ribbon(4, 3, ribbon(7, 3)); // National Champion
    pk6->ribbon(4, 4, ribbon(2, 5)); // World Champion

    // TODO
    //pk6->country
    //pk6->region
    //pk6->console

    // TODO
    pk6->currentHandler(1);
    //pk6->htName
    //pk6->htGender
    //pk6->geoRegion
    //pk6->geoCountry
    pk6->htIntensity(1);
    pk6->htMemory(4);
    pk6->htFeeling(rand() % 10);
    pk6->otFriendship(PersonalXYORAS::baseFriendship(pk6->species()));
    pk6->htFriendship(PersonalXYORAS::baseFriendship(pk6->species()));

    u32 shiny = 0;
    shiny = (PID() >> 16) ^ (PID() & 0xFFFF) ^ TID() ^ SID();
    if (shiny >= 8 && shiny < 16) // Illegal shiny transfer
        pk6->PID(pk6->PID() ^ 0x80000000);

    pk6->fixMoves();

    // Fix name strings TODO ???

    pk6->refreshChecksum();
    return std::unique_ptr<PKX>(pk6);
}

std::unique_ptr<PKX> PK5::previous(void)
{
    u8 dt[136];
    std::copy(data, data + 136, dt);

    // Clear nature field
    dt[0x41] = 0;

    std::unique_ptr<PKX> pk4(new PK4(dt));

    // Force normal Arceus form
    if (pk4->species() == 493)
    {
        pk4->alternativeForm(0);
    }

    pk4->nickname(nickname().c_str());
    pk4->otName(otName().c_str());
    pk4->heldItem(0);
    pk4->otFriendship(70);
    pk4->ball(ball());
    // met location ???
    // check illegal moves ???

    pk4->refreshChecksum();
    return pk4;
}