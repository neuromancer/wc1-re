"""Finder layout for dmgbuild; no Finder automation permissions required."""
from pathlib import Path

application = Path(defines["app"])
files = [str(application)]
symlinks = {"Applications": "/Applications"}
icon = str(application / "Contents/Resources/WC1.icns")
background = defines["background"]
format = "UDZO"
filesystem = "HFS+"
window_rect = ((120, 120), (640, 440))
default_view = "icon-view"
show_status_bar = False
show_tab_view = False
show_toolbar = False
show_pathbar = False
show_sidebar = False
show_icon_preview = False
arrange_by = None
icon_size = 100
text_size = 13
label_pos = "bottom"
# Hiding the extension adds FinderInfo to the signed bundle after signing.
# Leave it untouched so strict signature verification also passes in the DMG.
hide_extensions = []
icon_locations = {
    application.name: (160, 200),
    "Applications": (480, 200),
}
