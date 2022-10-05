/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#include "tchecker/system/param.hh"

namespace tchecker {

namespace system {

/* params_t */

void params_t::add_param(std::string const & name, tchecker::param_id_t size, tchecker::integer_t min,
                           tchecker::integer_t max, tchecker::system::attributes_t const & attr)
{
  _parameters.declare(name, size, min, max);
  for (tchecker::param_id_t i = 0; i < size; ++i)
    _parameters_attr.emplace_back(attr);
  assert(_parameters.size(tchecker::VK_FLATTENED) == _parameters_attr.size());
}

tchecker::system::attributes_t const & params_t::param_attributes(tchecker::param_id_t id) const
{
  if (id >= _parameters_attr.size())
    throw std::invalid_argument("Unknown integer variable identifier");
  return _parameters_attr[id];
}

bool params_t::is_param(std::string const & name) const { return _parameters.is_variable(name); }

} // end of namespace system

} // end of namespace tchecker
