#pragma once

#include <G4Material.hh>
#include <G4SystemOfUnits.hh>

#include <n4-constants.hh>

#include <optional>
#include <utility>
#include <vector>

namespace petmat {

const             double  OPTPHOT_MIN_ENERGY  {1.00*eV};
const             double  OPTPHOT_MAX_ENERGY  {8.21*eV};
const             double  OPTPHOT_MIN_WL      {c4::hc / OPTPHOT_MAX_ENERGY};
const             double  OPTPHOT_MAX_WL      {c4::hc / OPTPHOT_MIN_ENERGY};
const std::vector<double> OPTPHOT_ENERGY_RANGE{OPTPHOT_MIN_ENERGY, OPTPHOT_MAX_ENERGY};

////////////////////////////////////////////////////////////
// Scintillators spectra
std::pair<std::vector<double>, std::vector<double>>    csi_scint_spectrum();
std::pair<std::vector<double>, std::vector<double>> csi_tl_scint_spectrum();
std::pair<std::vector<double>, std::vector<double>>    bgo_scint_spectrum();
std::pair<std::vector<double>, std::vector<double>>   lyso_scint_spectrum();

////////////////////////////////////////////////////////////
// Scintillators
G4Material*    csi_with_properties(std::optional<double> maybe_scint_yield);
G4Material* csi_tl_with_properties(std::optional<double> maybe_scint_yield);
G4Material*   lyso_with_properties(std::optional<double> maybe_scint_yield);
G4Material*    bgo_with_properties(std::optional<double> maybe_scint_yield);

G4MaterialPropertiesTable*    csi_properties(std::optional<double> maybe_scint_yield);
G4MaterialPropertiesTable* csi_tl_properties(std::optional<double> maybe_scint_yield);
G4MaterialPropertiesTable*   lyso_properties(std::optional<double> maybe_scint_yield);
G4MaterialPropertiesTable*    bgo_properties(std::optional<double> maybe_scint_yield);

////////////////////////////////////////////////////////////
// Surrounding materials
G4Material*         air_with_properties();
G4Material*         esr_with_properties(std::optional<double> reflectivity);
G4Material*      teflon_with_properties(std::optional<double> reflectivity);
G4Material*     silicon_with_properties();
G4Material* optical_gel_with_properties();

G4MaterialPropertiesTable*         air_properties();
G4MaterialPropertiesTable*         esr_properties(std::optional<double> reflectivity);
G4MaterialPropertiesTable*      teflon_properties(std::optional<double> reflectivity);
G4MaterialPropertiesTable*     silicon_properties();
G4MaterialPropertiesTable* optical_gel_properties();

} // namespace petmat

namespace pm { using namespace petmat; }
