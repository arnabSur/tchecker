/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/refzg/refzg.hh"
#include "tchecker/variables/static_analysis.hh"

namespace tchecker {

namespace refzg {

/* Semantics functions */

tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::clock_constraint_container_t & invariant, tchecker::refzg::semantics_t & semantics,
                                 tchecker::integer_t spread, tchecker::refzg::initial_value_t const & initial_range)
{
  tchecker::state_status_t status = tchecker::ta::initial(system, vloc, intval, vedge, invariant, initial_range);
  if (status != tchecker::STATE_OK)
    return status;

  std::shared_ptr<tchecker::reference_clock_variables_t const> r = zone->reference_clock_variables();

  boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);

  tchecker::dbm::db_t * rdbm = zone->dbm();

  return semantics.initial(rdbm, *r, delay_allowed, invariant, spread);
}

tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::refzg::semantics_t & semantics,
                              tchecker::integer_t spread, tchecker::refzg::outgoing_edges_value_t const & edges)
{
  std::shared_ptr<tchecker::reference_clock_variables_t const> r = zone->reference_clock_variables();

  boost::dynamic_bitset<> const src_delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);

  tchecker::state_status_t status =
      tchecker::ta::next(system, vloc, intval, vedge, src_invariant, guard, reset, tgt_invariant, edges);
  if (status != tchecker::STATE_OK)
    return status;

  boost::dynamic_bitset<> const tgt_delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);
  boost::dynamic_bitset<> const sync_refclocks = tchecker::ta::sync_refclocks(system, *r, *vedge);

  tchecker::dbm::db_t * rdbm = zone->dbm();
  return semantics.next(rdbm, *r, src_delay_allowed, src_invariant, sync_refclocks, guard, reset, tgt_delay_allowed,
                        tgt_invariant, spread);
}

/* labels */

boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s)
{
  return tchecker::ta::labels(system, s);
}

/* is_valid_final */

bool is_valid_final(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s)
{
  return !s.zone().is_empty() && s.zone().is_synchronizable();
}

/* attributes */

void attributes(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s,
                std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system, s, m);
  m["zone"] = tchecker::to_string(s.zone(), system.clock_variables().flattened().index());
}

void attributes(tchecker::ta::system_t const & system, tchecker::refzg::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system, t, m);
}

/* refzg_impl_t */

refzg_impl_t::refzg_impl_t(std::shared_ptr<tchecker::ta::system_t const> const & system,
                           std::shared_ptr<tchecker::reference_clock_variables_t const> const & r,
                           std::shared_ptr<tchecker::refzg::semantics_t> const & semantics, tchecker::integer_t spread,
                           std::size_t block_size, std::size_t table_size)
    : _system(system), _r(r), _semantics(semantics), _spread(spread),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), block_size, _r, table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
  tchecker::variable_access_map_t va_map = tchecker::variable_access(*system);
  if (va_map.has_shared_variable() && (_r->refcount() > 1))
    throw std::invalid_argument("Zone graph with reference clocks is not sound for systems with shared variables");
}

tchecker::refzg::initial_range_t refzg_impl_t::initial_edges() { return tchecker::refzg::initial_edges(*_system); }

void refzg_impl_t::initial(tchecker::refzg::initial_value_t const & init_edge, std::vector<sst_t> & v)
{
  tchecker::refzg::state_sptr_t s = _state_allocator.construct();
  tchecker::refzg::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::initial(*_system, *s, *t, *_semantics, _spread, init_edge);
  v.push_back(std::make_tuple(status, s, t));
}

tchecker::refzg::outgoing_edges_range_t refzg_impl_t::outgoing_edges(tchecker::refzg::const_state_sptr_t const & s)
{
  return tchecker::refzg::outgoing_edges(*_system, s->vloc_ptr());
}

void refzg_impl_t::next(tchecker::refzg::const_state_sptr_t const & s, tchecker::refzg::outgoing_edges_value_t const & out_edge,
                        std::vector<sst_t> & v)
{
  tchecker::refzg::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::refzg::transition_sptr_t nextt = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::next(*_system, *nexts, *nextt, *_semantics, _spread, out_edge);
  v.push_back(std::make_tuple(status, nexts, nextt));
}

boost::dynamic_bitset<> refzg_impl_t::labels(tchecker::refzg::const_state_sptr_t const & s) const
{
  return tchecker::refzg::labels(*_system, *s);
}

bool refzg_impl_t::is_valid_final(tchecker::refzg::const_state_sptr_t const & s) const
{
  return tchecker::refzg::is_valid_final(*_system, *s);
}

void refzg_impl_t::attributes(tchecker::refzg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::refzg::attributes(*_system, *s, m);
}

void refzg_impl_t::attributes(tchecker::refzg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::refzg::attributes(*_system, *t, m);
}

void refzg_impl_t::share(tchecker::refzg::state_sptr_t & s) { _state_allocator.share(s); }

void refzg_impl_t::share(tchecker::refzg::transition_sptr_t & t) { _transition_allocator.share(t); }

std::shared_ptr<tchecker::ta::system_t const> const & refzg_impl_t::system_ptr() const { return _system; }

tchecker::ta::system_t const & refzg_impl_t::system() const { return *_system; }

tchecker::integer_t refzg_impl_t::spread() const { return _spread; }

/* refzg_t */

std::shared_ptr<tchecker::ta::system_t const> const & refzg_t::system_ptr() const { return ts_impl().system_ptr(); }

tchecker::ta::system_t const & refzg_t::system() const { return ts_impl().system(); }

tchecker::integer_t refzg_t::spread() const { return ts_impl().spread(); }

/* sharing_refzg_t */

std::shared_ptr<tchecker::ta::system_t const> const & sharing_refzg_t::system_ptr() const { return ts_impl().system_ptr(); }

tchecker::ta::system_t const & sharing_refzg_t::system() const { return ts_impl().system(); }

tchecker::integer_t sharing_refzg_t::spread() const { return ts_impl().spread(); }

/* factory */

// Factory of reference clock variables
static std::shared_ptr<tchecker::reference_clock_variables_t const>
reference_clocks_factory(enum tchecker::refzg::reference_clock_variables_type_t type, tchecker::ta::system_t const & system)
{
  switch (type) {
  case tchecker::refzg::SINGLE_REFERENCE_CLOCKS:
    return std::make_shared<tchecker::reference_clock_variables_t const>(
        tchecker::single_reference_clocks(system.clock_variables().flattened(), system.processes_count()));
  case tchecker::refzg::PROCESS_REFERENCE_CLOCKS: {
    tchecker::variable_access_map_t va_map = tchecker::variable_access(system);
    return std::make_shared<tchecker::reference_clock_variables_t const>(
        tchecker::process_reference_clocks(va_map, system.clock_variables().flattened(), system.processes_count()));
  }
  default:
    throw std::invalid_argument("Unknown reference clock variables type");
  }
}

/*!
 \brief Generic implementation of the factory
 \tparam REFZG : type of reference zone graph
*/
template <class REFZG>
REFZG * factory_generic(std::shared_ptr<tchecker::ta::system_t const> const & system,
                        enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                        enum tchecker::refzg::semantics_type_t semantics_type, tchecker::integer_t spread,
                        std::size_t block_size, std::size_t table_size)
{
  std::shared_ptr<tchecker::reference_clock_variables_t const> r(
      tchecker::refzg::reference_clocks_factory(refclocks_type, *system));
  std::shared_ptr<tchecker::refzg::semantics_t> semantics{tchecker::refzg::semantics_factory(semantics_type)};
  return new REFZG(system, r, semantics, spread, block_size, table_size);
}

tchecker::refzg::refzg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                   enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                                   enum tchecker::refzg::semantics_type_t semantics_type, tchecker::integer_t spread,
                                   std::size_t block_size, std::size_t table_size)
{
  return tchecker::refzg::factory_generic<tchecker::refzg::refzg_t>(system, refclocks_type, semantics_type, spread, block_size,
                                                                    table_size);
}

tchecker::refzg::sharing_refzg_t * factory_sharing(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                                   enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                                                   enum tchecker::refzg::semantics_type_t semantics_type,
                                                   tchecker::integer_t spread, std::size_t block_size, std::size_t table_size)
{
  return tchecker::refzg::factory_generic<tchecker::refzg::sharing_refzg_t>(system, refclocks_type, semantics_type, spread,
                                                                            block_size, table_size);
}

} // end of namespace refzg

} // end of namespace tchecker
