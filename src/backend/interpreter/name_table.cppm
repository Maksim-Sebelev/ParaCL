module;

export module interpreter_name_table;

import basic_name_table;

namespace ParaCL
{

export struct NameValue
{
  private:
    int value_;

  public:
    int value()
    {
        return value_;
    }
    NameValue() = default;
    NameValue(int value) : value_(value)
    {
    }
};

// export class NameTable : public BasicNameTable<NameValue>
// {
//   public:
//     NameTable() = default;
//     NameTable &operator=(const NameTable &) = default;
//     ~NameTable() = default;
// };

// export using NameTable = BasicNameTable<NameValue>;


} /* namespace ParaCL */
