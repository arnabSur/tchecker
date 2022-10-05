/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <stdexcept>

#include "tchecker/utils/ordering.hh"
#include "tchecker/variables/params.hh"

namespace tchecker {

/* param_info_t */

param_info_t::param_info_t(unsigned int size, tchecker::integer_t min, tchecker::integer_t max)
    : tchecker::size_info_t(size), _min(min), _max(max)
{
  if ((_min < 0) || (_min > _max))
    throw std::invalid_argument("expecting 0 <= min <= max");
}

/* parameters_t */

tchecker::param_id_t parameters_t::declare(std::string const & name, tchecker::param_id_t size,
                                                   tchecker::integer_t min, tchecker::integer_t max)
{
  tchecker::param_info_t info{size, min, max};
  return tchecker::array_variables_t<tchecker::param_id_t, tchecker::param_info_t, tchecker::param_index_t>::declare(name,
                                                                                                                        info);
}

/* params_valuation_t */

void params_valuation_destruct_and_deallocate(tchecker::params_valuation_t * v)
{
  tchecker::params_valuation_t::destruct(v);
  delete[] reinterpret_cast<char *>(v);
}

std::ostream & output(std::ostream & os, tchecker::params_valuation_t const & params_val,
                      tchecker::param_index_t const & index)
{
  auto const size = index.size();

  for (tchecker::param_id_t id = 0; id < size; ++id) {
    if (id > 0)
      os << ",";
    os << index.value(id) << "=" << params_val[id];
  }
  return os;
}

std::string to_string(tchecker::params_valuation_t const & params_val, tchecker::param_index_t const & index)
{
  std::stringstream sstream;
  output(sstream, params_val, index);
  return sstream.str();
}

int lexical_cmp(tchecker::params_valuation_t const & params_val1, tchecker::params_valuation_t const & params_val2)
{
  return tchecker::lexical_cmp(
      params_val1.begin(), params_val1.end(), params_val2.begin(), params_val2.end(),
      [](tchecker::integer_t i1, tchecker::integer_t i2) -> int { return (i1 < i2 ? -1 : (i1 == i2 ? 0 : 1)); });
}

} // end of namespace tchecker
