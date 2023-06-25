#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <functional>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression) try
        : ast_(ParseFormulaAST(expression)){
    }
    catch (const FormulaException& exc) {
        throw FormulaException(expression);
    }

    Value Evaluate(const SheetInterface& sheet) const override try {

        const std::function<double(Position)> get_cell_value = [&sheet](Position pos) {
            if (!pos.IsValid()) {
                throw FormulaError(FormulaError::Category::Ref);
            }
            const CellInterface* cell = sheet.GetCell(pos);
            if (!cell) {
                return 0.0;
            }
            CellInterface::Value val = cell->GetValue();
            if (std::holds_alternative<std::string>(val)) {
                const std::string str = std::get<std::string>(val);
                char *endptr;
                double result = std::strtod(str.c_str(), &endptr);
                if (*endptr != '\0' || (result == 0.0 && errno == ERANGE && !str.empty())) {
                    throw FormulaError(FormulaError::Category::Value);
                }
                return result;
            }
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            }
            throw std::get<FormulaError>(val);
        };

        return ast_.Execute(std::ref(get_cell_value));

    } catch (const FormulaError& exc) {
        return exc;
    }

    std::string GetExpression() const override try {
        std::stringstream str;
        ast_.PrintFormula(str);
        return str.str();
    } catch (const FormulaError& exc) {
        std::stringstream str;
        str << exc;
        return str.str();
    }

    std::vector<Position> GetReferencedCells() const override {
        auto tmp = ast_.GetCells();
        return {tmp.begin(), std::unique(tmp.begin(), tmp.end())};
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}
