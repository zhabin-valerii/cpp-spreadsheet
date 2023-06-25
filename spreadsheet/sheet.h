#pragma once

#include "cell.h"
#include "common.h"

#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <set>

class Cell;

class Sheet : public SheetInterface {
public:
    Sheet() = default;

    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;


        // Можете дополнить ваш класс нужными полями и методами


private:
        // Можете дополнить ваш класс нужными полями и методами
    Size size_;
    std::unordered_map<int, std::unordered_map<int, std::unique_ptr<Cell>>> data_;
    std::set<int> rows_;
    std::set<int> cols_;
};
