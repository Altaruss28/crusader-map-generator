# Crusader Map Generator

Procedural map generator for **Stronghold Crusader 1**.

---

## Installation and Usage

### Requirements

- Windows 7 or newer
- **Stronghold Crusader v1.41**  
  - Latest Steam or GOG release, or manually patched
  - English version (most Latin-alphabet languages should work)
  - **Stronghold Crusader Extreme is not supported**
- The game must be installed for the generator to function

Other game versions and localizations are not supported.

---

### Setup

1. **Download the latest release**

   Make sure to choose the correct build for your system.

   Extract the files and place them together in any folder on your PC.

2. **Configure the generator**

   Edit the `config.txt` file in the program directory.

   - A default configuration is provided
   - The config is validated before each run using very lenient rules
   - It is still possible to set impossible or nonsensical values

   Notes:
   - You may reorder variables freely
   - Every variable must exist and have a valid value
   - Full documentation of all variables is not provided  
     (experiment with values to see how they affect the output)

3. **Run the game**

   - If automatic map saving is enabled, you may stay in the main menu
   - Otherwise, create a new map in the map editor

4. **Run the generator**

   - Administrator permissions are required
   - Do **not** interact with the game while the generator is running  
     (doing so may corrupt output or crash the game)

   Antivirus notes:
   - If your antivirus quarantines the program, create an exception
   - Restore or reinstall the executable if necessary

---

## Licensing and Attribution

See [LICENSE](https://github.com/Altaruss28/crusader-map-generator/blob/main/LICENSE).

Anything fully or partially created using this program must be **clearly and prominently attributed** in any distribution or publication.

---

## Contact

- Discord: `altaruss28`
