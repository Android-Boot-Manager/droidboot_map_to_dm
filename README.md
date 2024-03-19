# Droiodboot map to dm

Map to dm is a tool to convert .map file generated by uncrypt to a dm table used by dmsetup.

While on modern android devices userdata is encrypted, sometimes (for example for dualboot) we might want to have unencrypted files on userdata. Android already have tool just for that, called uncrypt and typically used for OTA updates on A-only devices (to provide recovery with unencrypted OTA update). The tool works by unencrypting only blocks occupied by the given file, and creating map file, that lists used blocks. There is an issue when you try to edit the file (only edits without size xhange are excepted), when you change a file on f2fs at least it will use other blocks, presumably to proloung a flash lifetime by distributing load? This tool allows you to create a dmsetup compatible table based on map file, so you can create actual block device out of the file (mostly usefeul for images containing fs) and continue to use same blocks.
## Usage

``droidboot_map_to_dm path/to/map/file /path/to/output/table``

## Example

First uncrypt a file
``uncrypt /data/example /metadata/example.map``

Then convert map to dm table
``droidboot_map_to_dm /metadata/example.map /tmp/example.txt``

Now you can create a block device with dm mapper
``dmsetup create example /tmp/example.txt``