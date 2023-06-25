#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string& text);
    void Clear();
    void AddParent(Cell* cell);
    void EraseParent(Cell* parent);
    void ClearThisInChilds ();
    void InvalidateCache();
    void AddChilds(const std::vector<Position>& new_childs);
    bool IsCircular (std::vector<std::unordered_set<Cell*>>& cells_stack) const;

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    class Impl {
    public:
        Impl(Cell* cell);
        virtual CellInterface::Value GetValue() = 0;
        virtual std::string GetText() = 0;
        virtual ~Impl() = default;
        virtual void InvalidateCache() = 0;
        virtual void ClearThisInChilds() {}
        virtual void AddChild(Cell*) = 0;
        virtual bool IsCircular(std::vector<std::unordered_set<Cell*>>& cells_stack) const
                                                {return false;}
        virtual std::vector<Position> GetReferencedCells() const {return {};}
        void EraseParent(Cell* parent);
        Cell* this_cell_;
    };
    class EmptyImpl : public Impl {
    public:
        EmptyImpl(Cell* cell);
        CellInterface::Value GetValue() override;
        std::string GetText() override;
        void InvalidateCache() override;
        void AddChild(Cell*) override {}
        ~EmptyImpl();
    };

    class TextImpl : public Impl {
    public:
        TextImpl(std::string text, Cell* cell);
        CellInterface::Value GetValue() override;
        std::string GetText() override;
        void InvalidateCache() override;
        void AddChild(Cell*) override {}
        ~TextImpl();
    private:
        std::string value_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::unique_ptr<FormulaInterface>&& value,
                    SheetInterface& sheet, Cell* cell);
        CellInterface::Value GetValue() override;
        std::string GetText() override;
        ~FormulaImpl();
        void InvalidateCache() override;
        void ClearThisInChilds() override;
        void AddChild(Cell* cell) override;
        bool IsCircular(std::vector<std::unordered_set<Cell*>>& cells_stack) const override;
        std::vector<Position> GetReferencedCells() const override;
    private:
        std::unique_ptr<FormulaInterface> value_;
        std::optional<Value> cache_;
        SheetInterface& sheet_;
        std::unordered_set<Cell*> childs_;
    };
private:
    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
    std::unordered_set<Cell*> parents_;
};
