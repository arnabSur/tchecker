/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_PARAM_HH
#define TCHECKER_SYSTEM_PARAM_HH

#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/variables/params.hh"

/*!
 \file param.hh
 \brief Bounded parameters in systems
 */

namespace tchecker {

namespace system {

/*!
 \class params_t
 \brief Collection of bounded parameters
 */
class params_t {
public:
  /*!
   \brief Add a parameter
   \param name : variable name
   \param size : variable size (array of parameters)
   \param min : minimal value
   \param max : maximal value
   \param attr :  attributes
   \pre name is not a declared parameter
   \post variable name with minimal value min, maximal value max, and attributes attr has been added
   \throw std::invalid_argument : if name is a declared parameter
   */
  void add_param(std::string const & name, tchecker::param_id_t size = 1,
                  tchecker::integer_t min = std::numeric_limits<tchecker::integer_t>::min(),
                  tchecker::integer_t max = std::numeric_limits<tchecker::integer_t>::max(),
                  tchecker::system::attributes_t const & attr = tchecker::system::attributes_t());

  /*!
   \brief Accessor
   \param kind : kind of declared variable
   \return number of declared bounded parameters if kind = tchecker::VK_DECLARED,
   number of flattened bounded parameters if kind = tchecker::VK_FLATTENED
   */
  inline tchecker::param_id_t params_count(enum tchecker::variable_kind_t kind) const
  {
    return _parameters.size(kind);
  }

  /*!
   \brief Accessor
   \param name : variable name
   \return identifier of variable name
   \throw std::invalid_argument : if name is not am parameter
   */
  inline tchecker::param_id_t param_id(std::string const & name) const { return _parameters.id(name); }

  /*!
   \brief Accessor
   \param id : variable identifier
   \return name of parameter id
   \throw std::invalid_argument : if id is not an parameter
   */
  inline std::string const & param_name(tchecker::param_id_t id) const { return _parameters.name(id); }

  /*!
   \brief Accessor
   \param id : parameter identifier
   \return attributes of parameter id
   \throw std::invalid_argument : if id is not an parameter
   */
  tchecker::system::attributes_t const & param_attributes(tchecker::param_id_t id) const;

  /*!
   \brief Accessor
   \param name : parameter name
   \return true if name is a declared parameter, false otherwise
   */
  bool is_param(std::string const & name) const;

  /*!
   \brief Accessor
   \return parameters
   */
  inline tchecker::parameters_t const & parameters() const { return _parameters; }

private:
  tchecker::parameters_t _parameters;                    /*!< Parameters */
  std::vector<tchecker::system::attributes_t> _parameters_attr; /*!< Parameters attributes */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_INTVAR_HH
