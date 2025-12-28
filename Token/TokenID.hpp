
#ifndef TOKEN_ID_HPP
#define TOKEN_ID_HPP
#pragma once

#include <string>

enum class TTokenID : long long {
    
    /*************************** Токены по умолчанию:: ***************************/

    // Заглавные буквы A-Z
    A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H', I = 'I', J = 'J',
    K = 'K', L = 'L', M = 'M', N = 'N', O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T',
    U = 'U', V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

    // Строчные буквы a-z
    a = 'a', b = 'b', c = 'c', d = 'd', e = 'e', f = 'f', g = 'g', h = 'h', i = 'i', j = 'j',
    k = 'k', l = 'l', m = 'm', n = 'n', o = 'o', p = 'p', q = 'q', r = 'r', s = 's', t = 't',
    u = 'u', v = 'v', w = 'w', x = 'x', y = 'y', z = 'z',

    // Цифры 0-9
    Zero = '0', One = '1', Two = '2', Three = '3', Four = '4',
    Five = '5', Six = '6', Seven = '7', Eight = '8', Nine = '9',

    // Знаки препинания и символы
    Space = ' ',
    Exclamation = '!',   // !
    Quotation = '"',   // "
    Hash = '#',   // #
    Dollar = '$',   // $
    Percent = '%',   // %
    Ampersand = '&',   // &
    Apostrophe = '\'',  // '
    LeftParen = '(',   // (
    RightParen = ')',   // )
    LeftBrace = '{',   // {
    RightBrace = '}',   // }
    LeftBracket = '[',   // [
    RightBracket = ']',   // ]
    Asterisk = '*',   // *
    Plus = '+',   // +
    Comma = ',',   // ,
    Minus = '-',   // -
    Dot = '.',   // .
    Slash = '/',   // /
    Colon = ':',   // :
    Semicolon = ';',   // ;
    Less = '<',   // <
    Equals = '=',   // =
    Greater = '>',   // >
    Question = '?',   // ?
    At = '@',   // @
    Backslash = '\\',  // /
    Caret = '^',   // ^
    Underscore = '_',   // _
    Backtick = '`',   // `

    Pipe = '|',   // |

    Tilde = '~',   // ~

    // Управляющие символы (0–31, 127)
    Null = '\0',  // 0
    StartOfHeading = 1,     // SOH
    StartOfText = 2,     // STX
    EndOfText = 3,     // ETX
    EndOfTransmission = 4,  // EOT
    Enquiry = 5,     // ENQ
    Acknowledge = 6,     // ACK
    Bell = 7,     // '\a' (BEL)
    Backspace = 8,     // '\b' (BS)
    Tab = 9,     // '\t' (HT)
    LineFeed = 10,    // '\n' (LF)
    VerticalTab = 11,    // '\v' (VT)
    FormFeed = 12,    // '\f' (FF)
    CarriageReturn = 13,    // '\r' (CR)
    ShiftOut = 14,    // SO
    ShiftIn = 15,    // SI
    DataLinkEscape = 16,    // DLE
    DeviceControl1 = 17,    // DC1 (XON)
    DeviceControl2 = 18,    // DC2
    DeviceControl3 = 19,    // DC3 (XOFF)
    DeviceControl4 = 20,    // DC4
    NegativeAcknowledge = 21, // NAK
    SynchronousIdle = 22,   // SYN
    EndOfTransmissionBlock = 23, // ETB
    Cancel = 24,    // CAN
    EndOfMedium = 25,     // EM
    Substitute = 26,     // SUB
    Escape = 27,     // '\e' (ESC)
    FileSeparator = 28,     // FS
    GroupSeparator = 29,    // GS
    RecordSeparator = 30,   // RS
    UnitSeparator = 31,     // US
    Delete = 127,    // DEL
    
    // ===== Специальные токены =====
    FirstSpecialToken = 256,
    neof,          // Конец файла
    Unknown,      // Неизвестный символ

    /*************************** Токены лексера ***************************/

    // ===== Литералы =====
    Literal,
    Not,         // !
    Xor,         // ^ (исключающее ИЛИ)
    BitAnd,      // & (побитовое И)
    BitOr,       // | (побитовое ИЛИ)
    BitNot,      // ~ (побитовое НЕ)

    /*************************** Токены пре-лексера ***************************/

    IntegerLiteral,      // 123, -456, 0, 1000 (целочисленные значения в десятичной системе)
    FloatLiteral,        // литералы типа float: 3.14f, 2.5e10f, -0.5f, 1.0e-5f (обязательный суффикс f/F)
    DoubleLiteral,       // литералы типа double: 3.14, 2.5e10, -0.5, 1.0e-5 (по умолчанию или с суффиксом d/D)
    LongDoubleLiteral,   // литералы типа long double: 3.14l, 2.5e10L, -0.5l (обязательный суффикс l/L)
    CharLiteral,         // 'A', '\n', '#', '9' (одиночные символы в апострофах, включая escape-последовательности)
    WCharLiteral,        // L'A', L'Я', L'字' (wide-символы с префиксом L)
    StringLiteral,       // "text", "Hello World!", "123" (строки в двойных кавычках)
    WStringLiteral,      // L"text", L"Привет", L"中文文本" (wide-строки с префиксом L)
    IdentifierLiteral,   // name, age, counter, myVariable (имена переменных, функций, классов)
    HexLiteral,          // 0xFF, 0x1A3, 0xDEADBEEF (шестнадцатеричные числа с префиксом 0x)
    BinaryLiteral,       // 0b1010, 0b11001100, 0b1 (двоичные числа с префиксом 0b, C++14 и выше)
    BooleanLiteral,      // true, false (логические значения)
    NullptrLiteral,      // nullptr (нулевой указатель, C++11 и выше)

    // Присваивания
    Assign,      // =
    PlusAssign,  // +=
    MinusAssign, // -=
    MultAssign,  // *=
    DivAssign,   // /=
    ModAssign,   // %=
    AndAssign,   // &=
    OrAssign,    // |=
    XorAssign,   // ^=
    ShlAssign,   // Shl= аналог: <<=
    ShrAssign,   // Shr= аналог: >>=

    // Сравнения
    Equal,       // ==
    NotEqual,    // <>
    LessEqual,   // <=
    GreaterEqual,// >=

    
    // ===== Операторы (расширенный) =====
    // Арифметические
    Div,         // div
    Mod,         // mod

    // ===== Управляющие конструкции =====
    If, // if
    Else, // else 
    While, // while
    For, // for
    Try, // try
    Catch, // catch
    Case, // case
    Return, // return
    Break,       // break
    Continue,    // continue
    Switch,      // switch
    Default,     // default
    New,         // new
    Delete_,      // delete

    // ===== Объявление типов =====
    Class, // class
    Namespace, // namespace
    Enum, // enum
    Override, // ovveride
    Virtual, // virtual

    // ===== квалификаторы =====
    Const, // const

    // ===== Модификаторы =====
    Private, // private
    Protected, // protected
    Public, // public
    Static, // static
    Final, // final
    Operator, // operator

    // ===== Прочее =====
    Property, // __property
    FastCall, // __fastcall
    Var, // var
    Function, // function
    Access, // access

    Using, // using
    Pointer, // pointer
    Type, // type

    // ===== Комментарии =====
    LineComment,    // //
    BlockComment,   // /* */

    // ===== Директивы компилятора =====
    DefineDirective,
    IfDefDirective,
    IfNDefDirective,
    EndIfDirective,
    UndefDirective,
    IfDirective,
    ElseDirective,
    IncludeDirective,

    // ===== Операторы (расширенный) =====
    // Арифметические
    Inc,         // ++ (инкремент)
    Dec,         // -- (декремент)
    Power,       // ** (возведение в степень)

    // Логические/битовые
    And,         // &&
    Or,          // ||
    Shl,         // Shl аналог: <<
    Shr,         // Shl аналог: >>

    ScResOp,     // ::

    Arrow,     // ->
};

// Вспомогательная функция для преобразования enum в char
constexpr char constexprToChar(TTokenID s) {
    return static_cast<char>(s);
}

// Вспомогательная функция для преобразования char в enum
constexpr TTokenID constexprToTTokenID(char s) {
    return static_cast<TTokenID>(s);
}

#define GENERATE_NAME(name) \
case TTokenID::name: return #name; \

std::string NameTTokenID(TTokenID kind) {

    switch (kind)
    {
    GENERATE_NAME(A);
    GENERATE_NAME(B);
    GENERATE_NAME(C);
    GENERATE_NAME(D);
    GENERATE_NAME(E);
    GENERATE_NAME(F);
    GENERATE_NAME(G);
    GENERATE_NAME(H);
    GENERATE_NAME(I);
    GENERATE_NAME(J);
    GENERATE_NAME(K);
    GENERATE_NAME(L);
    GENERATE_NAME(M);
    GENERATE_NAME(N);
    GENERATE_NAME(O);
    GENERATE_NAME(P);
    GENERATE_NAME(Q);
    GENERATE_NAME(R);
    GENERATE_NAME(S);
    GENERATE_NAME(T);
    GENERATE_NAME(U);
    GENERATE_NAME(V);
    GENERATE_NAME(W);
    GENERATE_NAME(X);
    GENERATE_NAME(Y);
    GENERATE_NAME(Z);

    GENERATE_NAME(a);
    GENERATE_NAME(b);
    GENERATE_NAME(c);
    GENERATE_NAME(d);
    GENERATE_NAME(e);
    GENERATE_NAME(f);
    GENERATE_NAME(g);
    GENERATE_NAME(h);
    GENERATE_NAME(i);
    GENERATE_NAME(j);
    GENERATE_NAME(k);
    GENERATE_NAME(l);
    GENERATE_NAME(m);
    GENERATE_NAME(n);
    GENERATE_NAME(o);
    GENERATE_NAME(p);
    GENERATE_NAME(q);
    GENERATE_NAME(r);
    GENERATE_NAME(s);
    GENERATE_NAME(t);
    GENERATE_NAME(u);
    GENERATE_NAME(v);
    GENERATE_NAME(w);
    GENERATE_NAME(x);
    GENERATE_NAME(y);
    GENERATE_NAME(z);

    GENERATE_NAME(Zero);
    GENERATE_NAME(One);
    GENERATE_NAME(Two);
    GENERATE_NAME(Three);
    GENERATE_NAME(Four);
    GENERATE_NAME(Five);
    GENERATE_NAME(Six);
    GENERATE_NAME(Seven);
    GENERATE_NAME(Eight);
    GENERATE_NAME(Nine);

    GENERATE_NAME(Space);
    GENERATE_NAME(Exclamation);
    GENERATE_NAME(Quotation);
    GENERATE_NAME(Hash);
    GENERATE_NAME(Dollar);
    GENERATE_NAME(Percent);
    GENERATE_NAME(Ampersand);
    GENERATE_NAME(Apostrophe);
    GENERATE_NAME(LeftParen);
    GENERATE_NAME(RightParen);
    GENERATE_NAME(LeftBrace);
    GENERATE_NAME(RightBrace);
    GENERATE_NAME(LeftBracket);
    GENERATE_NAME(RightBracket);
    GENERATE_NAME(Asterisk);
    GENERATE_NAME(Plus);
    GENERATE_NAME(Comma);
    GENERATE_NAME(Minus);
    GENERATE_NAME(Dot);
    GENERATE_NAME(Slash);
    GENERATE_NAME(Colon);
    GENERATE_NAME(Semicolon);
    GENERATE_NAME(Less);
    GENERATE_NAME(Equals);
    GENERATE_NAME(Greater);
    GENERATE_NAME(Question);
    GENERATE_NAME(At);
    GENERATE_NAME(Backslash);
    GENERATE_NAME(Caret);
    GENERATE_NAME(Underscore);
    GENERATE_NAME(Backtick);
    GENERATE_NAME(Pipe);
    GENERATE_NAME(Tilde);
    GENERATE_NAME(Null);
    GENERATE_NAME(StartOfHeading);
    GENERATE_NAME(StartOfText);
    GENERATE_NAME(EndOfText);
    GENERATE_NAME(EndOfTransmission);
    GENERATE_NAME(Enquiry);
    GENERATE_NAME(Acknowledge);
    GENERATE_NAME(Bell);
    GENERATE_NAME(Backspace);
    GENERATE_NAME(Tab);
    GENERATE_NAME(LineFeed);
    GENERATE_NAME(VerticalTab);
    GENERATE_NAME(FormFeed);
    GENERATE_NAME(CarriageReturn);
    GENERATE_NAME(ShiftOut);
    GENERATE_NAME(ShiftIn);
    GENERATE_NAME(DataLinkEscape);
    GENERATE_NAME(DeviceControl1);
    GENERATE_NAME(DeviceControl2);
    GENERATE_NAME(DeviceControl3);
    GENERATE_NAME(DeviceControl4);
    GENERATE_NAME(NegativeAcknowledge);
    GENERATE_NAME(SynchronousIdle);
    GENERATE_NAME(EndOfTransmissionBlock);
    GENERATE_NAME(Cancel);
    GENERATE_NAME(EndOfMedium);
    GENERATE_NAME(Substitute);
    GENERATE_NAME(Escape);
    GENERATE_NAME(FileSeparator);
    GENERATE_NAME(GroupSeparator);
    GENERATE_NAME(RecordSeparator);
    GENERATE_NAME(UnitSeparator);
    GENERATE_NAME(Delete);
    GENERATE_NAME(FirstSpecialToken);
    GENERATE_NAME(neof);

    GENERATE_NAME(Unknown);

    GENERATE_NAME(Literal);
    GENERATE_NAME(IntegerLiteral);
    GENERATE_NAME(FloatLiteral);
    GENERATE_NAME(DoubleLiteral);
    GENERATE_NAME(LongDoubleLiteral);
    GENERATE_NAME(CharLiteral);
    GENERATE_NAME(WCharLiteral);
    GENERATE_NAME(StringLiteral);
    GENERATE_NAME(WStringLiteral);
    GENERATE_NAME(IdentifierLiteral);
    GENERATE_NAME(HexLiteral);
    GENERATE_NAME(BinaryLiteral);
    GENERATE_NAME(BooleanLiteral);
    GENERATE_NAME(NullptrLiteral);

    GENERATE_NAME(Assign);
    GENERATE_NAME(PlusAssign);
    GENERATE_NAME(MinusAssign);
    GENERATE_NAME(MultAssign);
    GENERATE_NAME(DivAssign);
    GENERATE_NAME(ModAssign);
    GENERATE_NAME(AndAssign);
    GENERATE_NAME(OrAssign);
    GENERATE_NAME(XorAssign);
    GENERATE_NAME(ShlAssign);
    GENERATE_NAME(ShrAssign);
    GENERATE_NAME(Equal);
    GENERATE_NAME(NotEqual);
    GENERATE_NAME(LessEqual);
    GENERATE_NAME(GreaterEqual);

    GENERATE_NAME(Div);
    GENERATE_NAME(Mod);

    GENERATE_NAME(If);
    GENERATE_NAME(Else);
    GENERATE_NAME(While);
    GENERATE_NAME(For);
    GENERATE_NAME(Try);
    GENERATE_NAME(Catch);
    GENERATE_NAME(Case);
    GENERATE_NAME(Return);
    GENERATE_NAME(Break);
    GENERATE_NAME(Continue);
    GENERATE_NAME(Switch);
    GENERATE_NAME(Default);
    GENERATE_NAME(New);
    GENERATE_NAME(Delete_);

    GENERATE_NAME(Class);
    GENERATE_NAME(Namespace);
    GENERATE_NAME(Enum);
    GENERATE_NAME(Override);
    GENERATE_NAME(Virtual);

    GENERATE_NAME(Const);

    GENERATE_NAME(Private);
    GENERATE_NAME(Protected);
    GENERATE_NAME(Public);
    GENERATE_NAME(Static);
    GENERATE_NAME(Final);
    GENERATE_NAME(Operator);

    GENERATE_NAME(Property);
    GENERATE_NAME(FastCall);
    GENERATE_NAME(Var);
    GENERATE_NAME(Function);
    GENERATE_NAME(Access);

    GENERATE_NAME(Using);
    GENERATE_NAME(Pointer);
    GENERATE_NAME(Type);

    GENERATE_NAME(LineComment);
    GENERATE_NAME(BlockComment);

    GENERATE_NAME(DefineDirective);
    GENERATE_NAME(IfDefDirective);
    GENERATE_NAME(IfNDefDirective);
    GENERATE_NAME(EndIfDirective);
    GENERATE_NAME(UndefDirective);
    GENERATE_NAME(IfDirective);
    GENERATE_NAME(ElseDirective);
    GENERATE_NAME(IncludeDirective);

    GENERATE_NAME(Inc);
    GENERATE_NAME(Dec);
    GENERATE_NAME(Power);
    GENERATE_NAME(And);
    GENERATE_NAME(Or);
    GENERATE_NAME(Not);
    GENERATE_NAME(Xor);
    GENERATE_NAME(BitAnd);
    GENERATE_NAME(BitOr);
    GENERATE_NAME(BitNot);
    GENERATE_NAME(Shl);
    GENERATE_NAME(Shr);
    GENERATE_NAME(ScResOp);
    GENERATE_NAME(Arrow);
    default:
        return "Unknow Token Name";
    }
}

struct LexToken {
    TTokenID type;
    std::string value;
    size_t line = 0;
    size_t column = 0;

    bool operator == (const LexToken& other) const {
        return type == other.type;
    };
};
#endif // TOKEN_ID_HPP
