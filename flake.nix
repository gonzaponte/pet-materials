{
  description = "Common materials used in PET simulations";

  inputs = {
    nain4  .url     = "github:jacg/nain4";
    nosys  .follows = "nain4/nosys";
    nixpkgs.follows = "nain4/nixpkgs";
  };

  outputs = inputs @ {
    nosys,
    nixpkgs, # <---- This `nixpkgs` still has the `system` e.g. legacyPackages.${system}.zlib
    ...
  }: let outputs = import ./flake/outputs.nix;
         systems = inputs.nain4.contains-systems.systems;
    in nosys (inputs // { inherit systems; }) outputs;

}
