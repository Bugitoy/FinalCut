# FINAL CUT: The Escape

A true-crime editing mystery in voxel style. You are the editor on a documentary about a prison escape:
put the security-camera clips in order on a timeline, switch camera angles, and hit **RTX ENHANCE** to find what
the ray-traced reflections and shadows are hiding. The producer wants a happy ending. How you cut it decides the
ending: the **Director's Cut** (the truth) or the **Producer's Cut** (the lie).

Started as an entry for the Epic AI MegaJam 2026 (theme: "Director's Cut") and now in development.

- Play it: https://kingmayhem.itch.io/finalcut
- Trailer: https://youtu.be/nhGwZWOCrjM

## Features
- Footage bin, 6-slot timeline, play / pause, several CCTV angles per clip
- Timestamp-gap puzzle that recovers a deleted clip
- RTX ENHANCE: rough CCTV feed vs. hardware ray-traced shadows and reflections with DLSS (Super Resolution,
  Ray Reconstruction, Frame Generation) and NVIDIA Reflex
- Voxel prison: cell block, hallway and control room, laundry, yard, street
- Characters made of bevelled voxel blocks driven by the UE5 mannequin skeleton (C++)

## Requirements
- Unreal Engine **5.8** (Windows)
- Visual Studio 2022 with the "Game development with C++" workload (the project has a small C++ module)
- An NVIDIA RTX GPU for the ray-traced ENHANCE mode and DLSS
- **NVIDIA DLSS plugin for UE 5.8** (not included - NVIDIA's licence does not allow redistributing it).
  Download it from NVIDIA's developer site (DLSS 4.5 Unreal Engine plugin, v8.9.0) and extract it into
  `Plugins/` so you get `Plugins/<plugin folder>/Plugins/DLSS/...`. The project still opens without it, but DLSS
  options and the NVIDIA features will be missing.

## Open the project
1. Clone with Git LFS installed (`git lfs install`), so the `.uasset` / `.umap` files are downloaded.
2. Right-click `FinalCut.uproject` -> **Generate Visual Studio project files** (optional), or just double-click
   it and let Unreal build the `FinalCut` module.
3. The game level is `Content/Maps/L_Prison`. Press **Play**.

## Layout
| Folder | What |
|---|---|
| `Source/FinalCut` | C++ game module: `AFCVoxelPerson` voxel characters, `UFCVoxelBodyData` body definitions |
| `Content/Maps` | `L_Prison`, the whole game level |
| `Content/UI` | editing-suite UI (`WBP_EditorUI`: timeline, playback, ENHANCE, endings), game mode, UI kit |
| `Content/Sequences` | the six CCTV clips `LS_C1`..`LS_C6` |
| `Content/Voxel` | room dioramas, props, vehicles, materials |
| `Content/Characters` | voxel body data assets, mannequin skeleton and animations |
| `Config` | project settings (ray tracing, DLSS defaults, input) |

## Credits
Fonts: Jersey 15 and VT323, both under the SIL Open Font License 1.1 (see `ThirdPartyNotices/`).
Built with Unreal Engine 5. NVIDIA, RTX and DLSS are trademarks of NVIDIA Corporation.

All rights reserved unless a licence file says otherwise.
