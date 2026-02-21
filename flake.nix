{
  description = "Build and dev env";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    systems = [
      # this OpenGL version is way newer than that of Mac
      "x86_64-linux"
      "aarch64-linux"
    ];
  in
  {
    devShells = (builtins.listToAttrs (builtins.map (system:
      let
        pkgs = nixpkgs.legacyPackages."${system}";
        nativeBuildInputs = with pkgs; [
          gnumake
          python313Packages.glad2
          pkgconf
        ];
        buildInputs = with pkgs; [
          glfw
        ];
        devBuildInputs = nativeBuildInputs ++ (with pkgs; [
          clang-tools
        ]);
      in
      {
        name = system;
        value = {
          default = pkgs.mkShell {
            inherit buildInputs;
            inherit nativeBuildInputs;
          };
          devenv = pkgs.mkShell {
            inherit buildInputs;
            nativeBuildInputs = devBuildInputs;
          };
        };
      }
    ) systems));
  };
}
