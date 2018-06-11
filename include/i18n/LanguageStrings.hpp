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

#ifndef LANGUAGESTRINGS_HPP
#define LANGUAGESTRINGS_HPP

#include <3ds.h>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "io.hpp"
#include "json.hpp"

enum Language
{
    JP = 1,
    EN,
    FR,
    IT,
    DE,
    UNUSED,
    ES,
    KO,
    ZH,
    TW,
    NL,
    PT
};

class LanguageStrings
{
protected:
    std::vector<std::string> abilities;
    std::vector<std::string> balls;
    std::vector<std::string> forms;
    std::vector<std::string> hps;
    std::vector<std::string> items;
    std::vector<std::string> moves;
    std::vector<std::string> natures;
    std::vector<std::string> speciess;
    nlohmann::json gui;

    std::vector<std::pair<std::string, int>> sortedItems;
    std::vector<std::pair<std::string, int>> sortedMoves;

    void load(Language lang, const std::string name, std::vector<std::string>& array);
    void loadGui(Language lang);

public:
    LanguageStrings(Language lang);
    std::string folder(Language lang) const;

    std::string sortedItem(u16 v) const;
    std::string sortedMove(u16 v) const;

    int item(std::string v) const;
    int move(std::string v) const;

    std::string ability(u8 v) const;
    std::string ball(u8 v) const;
    std::string form(u8 v) const;
    std::string hp(u8 v) const;
    std::string item(u16 v) const;
    std::string move(u16 v) const;
    std::string nature(u8 v) const;
    std::string species(u16 v) const;

    std::string localize(const std::string& v) const;
};

#endif