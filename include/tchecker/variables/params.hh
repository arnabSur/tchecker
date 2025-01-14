/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PARAMS_HH
#define TCHECKER_PARAMS_HH

#include <iostream>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/array.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/variables/variables.hh"

/*!
 \file params.hh
 \brief Parameters
 */

namespace tchecker {

// parameters

/*!
 \class param_info_t
 \brief Informations on parameter
 */
class param_info_t : public tchecker::size_info_t {
public:
  /*!
   \brief Constructor
   \param size : array size
   \param min : minimal value
   \param max : maximal value
   \pre size > 0  and  0 <= min <= max
   \throw std::invalid_argument : if precondition is violated
   */
  param_info_t(unsigned int size, tchecker::integer_t min, tchecker::integer_t max);

  /*!
   \brief Copy constructor
   */
  param_info_t(tchecker::param_info_t const &) = default;

  /*!
   \brief Move constructor
   */
  param_info_t(tchecker::param_info_t &&) = default;

  /*!
   \brief Destructor
   */
  ~param_info_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::param_info_t & operator=(tchecker::param_info_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::param_info_t & operator=(tchecker::param_info_t &&) = default;

  /*!
   \brief Accessor
   \return Minimal value
   */
  inline constexpr tchecker::integer_t min() const { return _min; }

  /*!
   \brief Accessor
   \return Maximal value
   */
  inline constexpr tchecker::integer_t max() const { return _max; }

private:
  tchecker::integer_t _min;           /*!< Minimal value */
  tchecker::integer_t _max;           /*!< Maximal value */
  tchecker::integer_t _initial_value; /*!< Initial value */
};

/*!
 \brief Index of bounded parameters
 */
using param_index_t = tchecker::index_t<tchecker::param_id_t, std::string>;

/*!
 \class parameters_t
 \brief Declaration of parameters
 */
class parameters_t
    : public tchecker::array_variables_t<tchecker::param_id_t, tchecker::param_info_t, tchecker::param_index_t> {
public:
  using tchecker::array_variables_t<tchecker::param_id_t, tchecker::param_info_t,
                                    tchecker::param_index_t>::array_variables_t;

  /*!
   \brief Declare a bounded parameter
   \param name : variable name
   \param size : variabel size (array)
   \param min : minimal value
   \param max : maximal value
   \param initial : initial value
   \pre 'name' is not a declared variable,
   size > 0,
   and 0 <= min <= max
   \post A bounded parameter with base name 'name', with size 'size', with minimal value 'min',
   wirh maximal value 'max', and with initial value 'initial' has been declared
   \throw std::invalid_argument : if the precondition is violated
   */
  tchecker::param_id_t declare(std::string const & name, tchecker::param_id_t size, tchecker::integer_t min,
                                tchecker::integer_t max);

protected:
  using tchecker::array_variables_t<tchecker::param_id_t, tchecker::param_info_t, tchecker::param_index_t>::declare;
};

/*!
 \brief Type of flat bounded parameters
 */
using flat_parameters_t =
    tchecker::flat_variables_t<tchecker::param_id_t, tchecker::param_info_t, tchecker::param_index_t>;

// Parameters valuation

/*!
 \brief Type of parameters array
 */
using params_array_t =
    tchecker::make_array_t<tchecker::integer_t, sizeof(tchecker::integer_t), tchecker::array_capacity_t<unsigned short>>;

/*!
 \class params_valuation_t
 \brief Valuation of parameters
 */
class params_valuation_t : public tchecker::params_array_t {
public:
  /*!
   \brief Assignment operator
   \param v : params valuation
   \post this is a copy of v
   \return this after assignment
   */
  tchecker::params_valuation_t & operator=(tchecker::params_valuation_t const & v) = default;

  /*!
   \brief Move assignment operator
   \param v : params valuation
   \post v has been moved to this
   \return this after assignment
   */
  tchecker::params_valuation_t & operator=(tchecker::params_valuation_t && v) = default;

  /*!
   \brief Accessor
   \return Size
   \note Size coincide with capacity for params valuations
   */
  inline constexpr typename tchecker::params_valuation_t::capacity_t size() const { return capacity(); }

  /*!
   \brief Construction
   \param args : arguments to a constructor of params_valuation_t
   \pre ptr points to an allocated zone of capacity at least
   allocation_size_t<params_valuation_t>::alloc_size(args)
   \post params_valuation_t(args) has been called on ptr
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args) { new (ptr) params_valuation_t(args...); }

  /*!
   \brief Destruction
   \param v : params valuation
   \post ~params_valuation_t() has been called on v
   */
  static inline void destruct(tchecker::params_valuation_t * v)
  {
    assert(v != nullptr);
    v->~params_valuation_t();
  }

protected:
  /*!
   \brief Constructor
   \param size : params valuation size
   \param value : initial value
   */
  params_valuation_t(unsigned short size, tchecker::integer_t value = 0)
      : params_array_t(std::make_tuple(size), std::make_tuple(value))
  {
  }

  /*!
   \brief Copy constructor
   \param v : params valuation
   \post this is a copy of v
   */
  params_valuation_t(tchecker::params_valuation_t const & v) = default;

  /*!
   \brief Move constructor
   \param v : params valuation
   \post v has been moved to this
   */
  params_valuation_t(tchecker::params_valuation_t && v) = default;

  /*!
   \brief Destructor
   */
  ~params_valuation_t() = default;
};

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class
 tchecker::params_valuation_t
 */
template <> class allocation_size_t<tchecker::params_valuation_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class
   tchecker::invars_valuation_t
   \return allocation size for objects of class tchecker::params_valuation_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args)
  {
    return tchecker::allocation_size_t<tchecker::params_array_t>::alloc_size(args...);
  }
};

/*!
 \brief Allocate and construct an params valuation
 \param size : params valuation size
 \param args : arguments to a constructor of tchecker::params_valuation_t
 \return an instance of params_valuation_t of size values constructed
 from args
 */
template <class... ARGS>
tchecker::params_valuation_t * params_valuation_allocate_and_construct(unsigned short size, ARGS &&... args)
{
  char * ptr = new char[tchecker::allocation_size_t<tchecker::params_valuation_t>::alloc_size(size)];

  tchecker::params_valuation_t::construct(ptr, args...);

  return reinterpret_cast<tchecker::params_valuation_t *>(ptr);
}

/*!
 \brief Destruct and deallocate an params valuation
 \param v : params valuation
 \pre v has been returned by params_valuation_allocate_and_construct
 \post v has been destructed and deallocated
 */
void params_valuation_destruct_and_deallocate(tchecker::params_valuation_t * v);

/*!
 \brief Output parameters valuation
 \param os : output stream
 \param params_val : parameters valuation
 \param index : an index of parameters
 \post params_val has been output to os with variable names from index
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::params_valuation_t const & params_val,
                      tchecker::param_index_t const & index);

/*!
 \brief Write parameters valuation to string
 \param params_val : parameters valuation
 \param index : an index of parameters
 \return An std::string representation of params_val using variable names from index
 */
std::string to_string(tchecker::params_valuation_t const & params_val, tchecker::param_index_t const & index);

/*!
 \brief Lexical ordering on integer valuations
 \param params_val1 : first parameter valuation
 \param params_val2 : second parameter valuation
 \return 0 if params_val1 and params_val2 are equal, a negative value if params_val1 is smaller than params_val2 w.r.t.
 lexical ordering, and a positive value otherwise.
 */
int lexical_cmp(tchecker::params_valuation_t const & params_val1, tchecker::params_valuation_t const & params_val2);

} // end of namespace tchecker

#endif // TCHECKER_INTVARS_HH
