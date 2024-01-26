#include <pet-materials.hh>

#include <n4-material.hh>
#include <n4-sequences.hh>
#include <n4-stats.hh>

#include <G4MaterialPropertiesTable.hh>
#include <G4SystemOfUnits.hh>

namespace petmat {

////////////////////////////////////////////////////////////
// Scintillators spectra
std::pair<std::vector<double>, std::vector<double>> csi_scint_spectrum() {
  // From R. Soleti
  auto energies = n4::const_over(c4::hc/nm, {460,   400,   380,   340,   320,   300,   280,   260}); // wl in nm
  auto spectrum = n4::scale_by  (0.01     , {  4,    10,    29,    67,    88,    29,    10,     2});
  auto spectrum_norm = n4::stats::sum(spectrum);
  spectrum = n4::map<double>([&] (auto s){return  s/spectrum_norm;}, spectrum);
  return {std::move(energies), std::move(spectrum)};
}

std::pair<std::vector<double>, std::vector<double>> csi_tl_scint_spectrum() {
  // From R. Soleti
  auto energies = n4::const_over(c4::hc/nm, {722, 678, 611, 554, 523, 493, 437, 351}); // wl in nm
  auto spectrum = n4::scale_by  (0.01     , { 12,  29,  70,  98,  87,  56,  19,   1});
  auto spectrum_norm = n4::stats::sum(spectrum);
  spectrum = n4::map<double>([&] (auto s){return  s/spectrum_norm;}, spectrum);
  return {std::move(energies), std::move(spectrum)};
}

std::pair<std::vector<double>, std::vector<double>> lyso_scint_spectrum() {
  auto energies = n4::const_over(c4::hc/nm, {600  , 504  , 477  , 452  , 417, 410  , 402, 389  , 380});
  auto spectrum = n4::scale_by  (0.01     , {  0.7,   6.3,  16.9,  45.7,  96,  96.4,  95,  37.7,   0});
  auto spectrum_norm = n4::stats::sum(spectrum);
  spectrum = n4::map<double>([&] (auto s){return  s/spectrum_norm;}, spectrum);
  return {std::move(energies), std::move(spectrum)};
}

std::pair<std::vector<double>, std::vector<double>> bgo_scint_spectrum() {
  auto energies = n4::const_over(c4::hc/nm, {713, 638, 584  , 529  , 476  , 443  , 407  , 367});
  auto spectrum = n4::scale_by  (0.01     , {  1,   3,   5.6,  10.3,  12.6,   9.6,   4.0,   7});
  auto spectrum_norm = n4::stats::sum(spectrum);
  spectrum = n4::map<double>([&] (auto s){return  s/spectrum_norm;}, spectrum);
  return {std::move(energies), std::move(spectrum)};
}

////////////////////////////////////////////////////////////
// Scintillators
std::vector<double> csi_rindex() {
  // rindex: values taken from "Optimization of Parameters for a
  // CsI(Tl) Scintillator Detector Using GEANT4-Based Monte Carlo..."
  // by Mitra et al (mainly page 3) latest numbers from
  // https://refractiveindex.info/?shelf=main&book=CsI&page=Querry
  return {1.766, 1.794, 1.806, 1.845, 1.867, 1.902, 1.955, 2.043};
}

G4MaterialPropertiesTable* csi_properties(std::optional<double> maybe_scint_yield) {
  //  scint: values from Fig. 2 in "A New Scintillation Material: Pure CsI with 10ns Decay Time" by Kubota et al (these are approximate...)
  // must be in increasing ENERGY order (decreasing wavelength) for scintillation to work properly

  auto [energies, spectrum] = csi_scint_spectrum();
  // Values from "Temperature dependence of pure CsI: scintillation light yield and decay time" by Amsler et al
  // "cold" refers to ~77K, i.e. liquid nitrogen temperature
  double scint_yield = maybe_scint_yield.value_or(50'000 / MeV); // 50000 / MeV in cold
  double time_fast   =  1015 * ns; // only one component at cold temps!
  double time_slow   =  1015 * ns;
  return n4::material_properties()
    .add("RINDEX"                    , energies, csi_rindex())
    .add("SCINTILLATIONCOMPONENT1"   , energies, spectrum)
    .add("SCINTILLATIONCOMPONENT2"   , energies, spectrum)
    .add("ABSLENGTH"                 , energies, 5*m)
    .add("SCINTILLATIONTIMECONSTANT1", time_fast)
    .add("SCINTILLATIONTIMECONSTANT2", time_slow)
    .add("SCINTILLATIONYIELD"        , scint_yield)
    .add("SCINTILLATIONYIELD1"       ,     0.57   )
    .add("SCINTILLATIONYIELD2"       ,     0.43   )
    .add("RESOLUTIONSCALE"           ,     1.0    )
    .done();
}

G4Material* csi_with_properties(std::optional<double> maybe_scint_yield) {
  auto csi = n4::material("G4_CESIUM_IODIDE");
  csi -> GetIonisation() -> SetBirksConstant(0.00152 * mm/MeV);
  csi -> SetMaterialPropertiesTable(csi_properties(maybe_scint_yield));
  return csi;
}

G4MaterialPropertiesTable* csi_tl_properties(std::optional<double> maybe_scint_yield) {
  auto [energies, spectrum] = csi_tl_scint_spectrum();
  double scint_yield = maybe_scint_yield.value_or(50'000 / MeV);
  auto abslength     = n4::scale_by(cm, {42, 40, 37, 33, 31, 30, 26, 12});
  return n4::material_properties()
    .add("RINDEX"                    , energies, csi_rindex())
    .add("SCINTILLATIONCOMPONENT1"   , energies, spectrum)
    .add("ABSLENGTH"                 , energies, abslength)
    .add("SCINTILLATIONTIMECONSTANT1", 500 * ns)
    .add("SCINTILLATIONYIELD"        , scint_yield)
    .add("SCINTILLATIONYIELD1"       ,   1.0   )
    .add("RESOLUTIONSCALE"           ,   1.0   )
    .done();
}

G4Material* csi_tl_with_properties(std::optional<double> maybe_scint_yield) {
  auto csi = n4::material("G4_CESIUM_IODIDE");
  csi -> GetIonisation() -> SetBirksConstant(0.00152 * mm/MeV);
  csi -> SetMaterialPropertiesTable(csi_tl_properties(maybe_scint_yield));
  return csi;
}

G4MaterialPropertiesTable* bgo_properties(std::optional<double> maybe_scint_yield) {
  // Refractive index, scintillation spectrum and time constant taken from
  //   https://jnm.snmjournals.org/content/jnumed/41/6/1051.full.pdf
  auto [energies, spectrum] = csi_scint_spectrum();
  //double scint_yield = maybe_scint_yield.value_or(8'500 / MeV); // According to Wikipedia
  double scint_yield = maybe_scint_yield.value_or(9'000 / MeV); // Roberto
  return n4::material_properties()
    .add("RINDEX"                    , OPTPHOT_ENERGY_RANGE, 2.15)
    .add("ABSLENGTH"                 , OPTPHOT_ENERGY_RANGE, 0.5*m)
    .add("SCINTILLATIONCOMPONENT1"   , energies, spectrum)
    .add("SCINTILLATIONTIMECONSTANT1", 300 * ns)
    .add("SCINTILLATIONYIELD"        , scint_yield)
    .add("RESOLUTIONSCALE"           ,     1.0    ) // TODO what is RESOLUTIONSCALE ?
    .done();
}

G4Material* bgo_with_properties(std::optional<double> maybe_scint_yield) {
  auto bgo = n4::material("G4_BGO");
  bgo -> SetMaterialPropertiesTable(bgo_properties(maybe_scint_yield));
  return bgo;
}

G4Material* lyso_material() {
  // TODO get better LYSO material property data
  // https://arxiv.org/pdf/2207.06696.pdf
  auto density = 7.1 * g/cm3;
  auto               [ fLu ,  fY  ,  fSi ,  fO  ] =
      std::make_tuple(0.714, 0.040, 0.064, 0.182);

  return nain4::material_from_elements_F("LYSO", density, {.state=kStateSolid},
                                         {{"Lu", fLu}, {"Y", fY}, {"Si", fSi}, {"O", fO}});
}

G4MaterialPropertiesTable* lyso_properties(std::optional<double> maybe_scint_yield) {
  auto [energies, spectrum] = lyso_scint_spectrum();
  double scint_yield        = maybe_scint_yield.value_or(25'000 / MeV); // Roberto
  return n4::material_properties()
    .add("RINDEX"                    , OPTPHOT_ENERGY_RANGE, 1.82)
    .add("ABSLENGTH"                 , OPTPHOT_ENERGY_RANGE, 0.5*m)
    .add("SCINTILLATIONCOMPONENT1"   , energies, spectrum)
    .add("SCINTILLATIONTIMECONSTANT1", 40 * ns)
    .add("SCINTILLATIONYIELD"        , scint_yield)
    .add("RESOLUTIONSCALE"           ,     1.0    ) // TODO what is RESOLUTIONSCALE ?
    .done();
}

G4Material* lyso_with_properties(std::optional<double> maybe_scint_yield) {
  auto lyso = lyso_material();
  lyso -> SetMaterialPropertiesTable(lyso_properties(maybe_scint_yield));
  return lyso;
}


////////////////////////////////////////////////////////////
// Surrounding materials
G4Material* air_with_properties() {
  auto air = n4::material("G4_AIR");
  air -> SetMaterialPropertiesTable(air_properties());
  return air;
}

G4Material* esr_with_properties(std::optional<double> maybe_reflectivity) {
  auto esr = n4::material("G4_POLYVINYL_ALCOHOL");
  esr -> SetMaterialPropertiesTable(esr_properties(maybe_reflectivity));
  return esr;
}

G4Material* teflon_with_properties(std::optional<double> maybe_reflectivity) {
  auto teflon = n4::material("G4_TEFLON");
  teflon -> SetMaterialPropertiesTable(teflon_properties(maybe_reflectivity));
  return teflon;
}

G4MaterialPropertiesTable* air_properties() {
  return n4::material_properties()
    .add("RINDEX", OPTPHOT_ENERGY_RANGE, 1.)
    .done();
}
G4MaterialPropertiesTable* esr_properties(std::optional<double> maybe_reflectivity) {
  auto reflectivity = maybe_reflectivity.has_value() ? maybe_reflectivity.value() : 0.99;

  return n4::material_properties()
    .add("RINDEX"               , OPTPHOT_ENERGY_RANGE, 1.41)
    .add("REFLECTIVITY"         , OPTPHOT_ENERGY_RANGE, reflectivity)
    .add("SPECULARLOBECONSTANT" , OPTPHOT_ENERGY_RANGE, 0.0)
    .add("SPECULARSPIKECONSTANT", OPTPHOT_ENERGY_RANGE, 0.0)
    .add("BACKSCATTERCONSTANT"  , OPTPHOT_ENERGY_RANGE, 0.0)
    .done();
}

G4MaterialPropertiesTable* teflon_properties(std::optional<double> maybe_reflectivity) {
  // Values could be taken from "Optical properties of Teflon AF amorphous fluoropolymers" by Yang, French & Tokarsky (using AF2400, Fig.6)
  // but are also stated in the same paper as above
  auto energy       = n4::scale_by(  eV, {OPTPHOT_MIN_ENERGY/eV,  2.8,  3.5,  4.0,  6.0,  7.2, OPTPHOT_MAX_ENERGY/eV});
  auto reflectivity = n4::scale_by(0.01, {                   98, 98  , 98  , 98  , 72  , 72  , 72});
  if (maybe_reflectivity.has_value()) {
    reflectivity = std::vector<double>(energy.size(), maybe_reflectivity.value());
  }

  return n4::material_properties()
    .add("RINDEX"               , energy, 1.41)
    .add("REFLECTIVITY"         , energy, reflectivity)
    .add("SPECULARLOBECONSTANT" , OPTPHOT_ENERGY_RANGE, 0.0)
    //.add("SPECULARSPIKECONSTANT", OPTPHOT_ENERGY_RANGE, 0.0)
    .add("BACKSCATTERCONSTANT"  , OPTPHOT_ENERGY_RANGE, 0.0)
    .done();
}

// TODO: implement E dependence from
// https://refractiveindex.info/?shelf=main&book=Si&page=Aspnes
// TODO: implement absorption length from
// https://www.researchgate.net/figure/Absorption-length-1-a-as-a-function-of-wavelength-l-for-Silicon-Actually-not-all-the_fig5_221914026
G4Material* silicon_with_properties() {
  auto si  = n4::material("G4_Si");
  auto mpt = n4::material_properties()
    .add("RINDEX"    , OPTPHOT_ENERGY_RANGE, 4.32)
    .add("EFFICIENCY", OPTPHOT_ENERGY_RANGE, 1) // full absorption at the sipm surface
    .add("ABSLENGTH" , OPTPHOT_ENERGY_RANGE, 10 * nm)
    .done();
  si -> SetMaterialPropertiesTable(mpt);
  return si;
}


G4Material* optical_gel() {
  // Silicone resin with a methyl group
  // (https://en.wikipedia.org/wiki/Silicone_resin)
  auto name = "OpticalSilicone";
  auto density = 1.05*g/cm3;
  return n4::material_from_elements_N( name
                                     , density
                                     , {.state=kStateSolid}
                                     , { {"H", 3}, {"C", 1}, {"O", 1}, {"Si", 1} });
}

G4MaterialPropertiesTable* optical_gel_properties() {
  // gel NyoGel OCK-451
  auto r_index_fn = [] (auto e) {
    auto wl = c4::hc / e;
    static const auto  const_term = 1.4954;
    static const auto square_term = 0.008022 * um*um;
    return const_term + square_term/wl/wl;
  };
  auto [r_energies, r_index] = n4::interpolate(r_index_fn, 100, OPTPHOT_MIN_ENERGY, OPTPHOT_MAX_ENERGY);

  // ABSORPTION LENGTH
  // Values estimated from printed plot (to be improved).
  const auto very_long = 1e6;
  auto abs_energies = n4::scale_by(eV, {OPTPHOT_MIN_ENERGY/eV,
                                           1.70,   1.77,    2.07,   2.48,   2.76,  2.92,
                                           3.10,   3.31,    3.54,   3.81,   4.13,
                                        OPTPHOT_MAX_ENERGY/eV});
  auto abs_length = n4::scale_by(mm, {very_long,
                                      very_long, 1132.8, 1132.8 , 1132.8, 666.17, 499.5,
                                          399.5,  199.5,  132.83,   99.5,   4.5 ,
                                        4.5});

  return n4::material_properties()
    .add("RINDEX", r_energies, r_index)
    .add("ABSLENGTH", abs_energies, abs_length)
    .done();
}

G4Material* optical_gel_with_properties() {
  auto gel = optical_gel();
  gel -> SetMaterialPropertiesTable(optical_gel_properties());
  return gel;
}

std::pair<std::vector<double>, std::vector<double>> sipm_pde() {
  auto energies = n4::const_over(c4::hc/nm, {OPTPHOT_MAX_WL/nm, 809.722, 675.000, 587.500, 494.444, 455.556, 422.222, 395.833,
                                                       366.667, 344.444, 311.111, 293.056, 288.889, 279.167, OPTPHOT_MIN_WL/nm});
  auto pde      = n4::scale_by  (0.01     , {                0,   0.87 ,  19.2  ,  31.1  ,  46.7  ,  51.1  ,  50.2  ,  46.9  ,
                                                        40.6  ,  39.3  ,  32.4  ,  18.0  ,   4.8  ,   2.0  ,                 0});

  return {energies, pde};
}

} // namespace petmat
