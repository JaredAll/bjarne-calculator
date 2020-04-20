//This code comes from The C++ Programming Language, 4th Edition, by Bjarne Stroustrup,
//Chapter 10, Section 2. Modified by Jared Allen. 

#include <iostream>
#include <string>
#include <map>
#include <cctype>

using namespace std;

enum class Kind : char 
{
  name = 'n',
  number = '#',
  end = 'e',
  plus = '+',
  minus = '-',
  multiplication = '*',
  division = '/',
  print = ';',
  assign = '=',
  left_parentheses = '(',
  right_parentheses = ')'

};

ostream& operator<<( ostream& output_stream, Kind kind )
{
  output_stream << static_cast<char>(kind) << endl;
  return output_stream;
}


struct Token
{
  Kind kind;
  string string_value;
  double number_value;
};

int number_of_errors;

void error( const string& error_string )
{
  number_of_errors++;
  cerr << "error: " << error_string << endl;
}

class TokenStream
{
public:
  TokenStream( istream& param_input_stream )
    : input_stream( &param_input_stream ),
      owns_input_stream( false )
  {
  }

  TokenStream( istream* param_input_stream )
    : input_stream( param_input_stream ),
      owns_input_stream( true )
  {
  }

  Token get_token()
  {
    char input_character = 0;

    if( input_stream -> get( input_character ))
    {
      while( input_character != '\n' && isspace( input_character ) )
      {
        input_stream -> get( input_character );
      }
      
      switch( input_character )
      {
      case 0:
        token = { Kind::end };
        break;
      case ';':
      case '\n':
        token = { Kind::print };
        break;
      case '*':
      case '/':
      case '+':
      case '-':
      case '(':
      case ')':
      case '=':
        token = { static_cast<Kind>( input_character ) };
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '.':
        input_stream -> putback( input_character );
        *input_stream >> token.number_value;
        token.kind = Kind::number;
        break;
      default:
        if( isalpha( input_character ) )
        {
          token.string_value = input_character;
          bool next_character_read;
          if( input_stream -> get( input_character ) )
          {
            next_character_read = true;
          }
          else
          {
            next_character_read = false;
          }
          while( next_character_read && isalnum( input_character ) )
          {
            token.string_value += input_character;
            if( input_stream -> get( input_character ) )
            {
              next_character_read = true;
            }
            else
            {
              next_character_read = false;
            }
          }
          if( next_character_read && !isalnum( input_character ) )
          {
            input_stream -> putback( input_character );  
          }
          token.kind = Kind::name;
        }
        else
        {
          error( "bad token" );
          token = { Kind::print };
        }
      }
    }
    else
    {
      token = { Kind::end };
    }
    return token;
  }
  
  const Token& current_token()
  {
    return token;
  }

private:
  istream* input_stream;
  bool owns_input_stream;
  Token token { Kind::end };
};

map<string, double> table;

TokenStream token_stream {cin};

double term( bool read_token );

double expression( bool read_token )
{
  double left = term( read_token );

  while( token_stream.current_token().kind == Kind::plus ||
         token_stream.current_token().kind == Kind::minus )
  {
    switch( token_stream.current_token().kind )
    {
    case Kind::plus:
      left += term( true );
      break;
    case Kind::minus:
      left -= term( true );
      break;
    default:
      break;
    }
  }
  return left;
}

double primary( bool read_token );

double term( bool read_token )
{
  double left = primary( read_token );

  while( token_stream.current_token().kind == Kind::multiplication ||
         token_stream.current_token().kind == Kind::division )
  {
    switch( token_stream.current_token().kind )
    {
    case Kind::multiplication:
      left *= primary( true );
      break;
    case Kind::division:
      if( double d = primary( true ) )
      {
        left /= d;
      }
      else
      {
        error( "divide by 0" );
      }
    default:
      break;
    }
  }  
  return left;
}

double primary( bool read_token )
{
  if( read_token )
  {
    token_stream.get_token();
  }

  double value;
  string name = "";
  switch( token_stream.current_token().kind )
  {
  case Kind::number:
    value = token_stream.current_token().number_value;
    token_stream.get_token();
    break;
  case Kind::name:
    name = token_stream.current_token().string_value;
    value = table[ name ];
    if( token_stream.get_token().kind == Kind::assign )
    {
      value = expression( true );
      table[ name ] = value;
    }
    break;
  case Kind::minus:
    value = -primary( true );
    break;
  case Kind::left_parentheses:
    value = expression( true );
    if( token_stream.current_token().kind != Kind::right_parentheses )
    {
      error( "')' expected");
    }
    else
    {
      token_stream.get_token();
    }
    break;
  default:
    error( "primary expected" );
  }
  return value;
}

void calculate()
{
  token_stream.get_token();
  while( token_stream.current_token().kind != Kind::end )

  {
    if( token_stream.current_token().kind != Kind::print )
    {
      cout << expression( false ) << endl;  
    }
    token_stream.get_token();
  }
}

int main()
{
  table[ "b" ] = 7;

  calculate();
  return number_of_errors;
}
