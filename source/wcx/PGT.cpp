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

#include "PGT.hpp"

PGT::PGT(u8* pgt)
{
    std::copy(pgt, pgt + length, data);
    u8 pk4Data[136];
    std::copy(pgt + 0x8, pgt + 0x8 + 136, pk4Data);
    pokemonData = new PK4(pk4Data, true);
}

PGT::~PGT() { delete pokemonData; }

u16 PGT::ID(void) const { return 0; }

std::string PGT::title(void) const { return "Wondercard"; }

bool PGT::power(void) const { return false; }

u8 PGT::generation(void) const { return 4; }

u8 PGT::type(void) const { return *(u8*)(data); }

bool PGT::bean(void) const { return false; }

bool PGT::BP(void) const { return false; }

bool PGT::item(void) const { return type() == 3 || type() == 8 || type() == 9 || type() == 10; }

// Pokemon, egg, or Manaphy egg
bool PGT::pokemon(void) const { return type() == 1 || type() == 2 || type() == 7; }

u16 PGT::object(void) const { return *(u16*)(data + 0x4); }

u8 PGT::flags(void) const { return *(u8*)(data + 0x3); }

bool PGT::multiObtainable(void) const { return false; }

u32 PGT::year(void) const { return 0; }

u32 PGT::month(void) const { return 0; }

u32 PGT::day(void) const { return 0; }

u32 PGT::rawDate(void) const { return 0; }

void PGT::rawDate(u32 v) { (void)v; }

u8 PGT::cardLocation(void) const { return 0; }

bool PGT::used(void) const { return false; }

u8 PGT::ball(void) const { return pokemonData->ball(); }

u16 PGT::heldItem(void) const { return pokemonData->heldItem(); }

bool PGT::shiny(void) const { return pokemonData->shiny(); }

u8 PGT::PIDType(void) const { return pokemonData->shiny() ? 2 : 0; }

u16 PGT::TID(void) const { return pokemonData->TID(); }

u16 PGT::SID(void) const { return pokemonData->SID(); }

u16 PGT::move(u8 index) const { return pokemonData->move(index); }

u16 PGT::species(void) const { return pokemonData->species(); }

u8 PGT::gender(void) const { return pokemonData->gender(); }

std::string PGT::otName(void) const { return flags() != 0 ? pokemonData->otName() : "Your OT Name"; }

u8 PGT::level(void) const { return pokemonData->level(); }

u32 PGT::PID(void) const { return pokemonData->PID(); }

bool PGT::ribbon(u8 category, u8 index) const { return pokemonData->ribbon(category, index); }

u8 PGT::alternativeForm(void) const { return pokemonData->alternativeForm(); }

u8 PGT::language(void) const { return pokemonData->language(); }

std::string PGT::nickname(void) const { return pokemonData->nickname(); }

u8 PGT::nature(void) const { return pokemonData->nature(); }

u8 PGT::abilityType(void) const { return pokemonData->abilityNumber(); }

u16 PGT::ability(void) const { return pokemonData->ability(); }

u16 PGT::eggLocation(void) const { return pokemonData->eggLocation(); }

u16 PGT::metLocation(void) const { return pokemonData->metLocation(); }

u8 PGT::metLevel(void) const { return pokemonData->metLevel(); }

u8 PGT::contest(u8 index) const { return pokemonData->contest(index); }

u8 PGT::iv(u8 index) const { return pokemonData->iv(index);}

bool PGT::egg(void) const { return pokemonData->egg(); }

u16 PGT::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form = alternativeForm();
    u8 formcount = PersonalDPPtHGSS::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies = PersonalDPPtHGSS::formStatIndex(tmpSpecies);
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