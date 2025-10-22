-- SPDX-License-Identifier: GPL-2.0-or-later
with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;
use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is
   -- Sophos SG230 Rev 2 : HDMI sur DP-B (DDI-B)
   -- D'après le VBT : EFP1 = DP-B avec support HDMI/DP/DVI
   ports : constant Port_List :=
     (DP2,      -- DP-B = port HDMI principal (EFP1)
      others => Disabled);
end GMA.Mainboard;