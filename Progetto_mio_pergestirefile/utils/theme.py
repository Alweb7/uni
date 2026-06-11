# Palette colori e stili professionali Glassmorphism

class Colors:
    # Primari
    PRIMARY = "#14b8a6"      # Teal
    PRIMARY_HOVER = "#0f766e"
    SECONDARY = "#f59e0b"    # Amber
    ERROR = "#ef4444"        # Rosso sofisticato
    WARNING = "#f97316"      # Arancione soft
    SUCCESS = "#22c55e"

    # Background
    BG_DARK = "#0d1117"
    BG_GLASS = "#171c24"
    BG_GLASS_ALPHA = "rgba(23, 28, 36, 0.7)"
    SURFACE = "#111820"
    SURFACE_ALT = "#1d2630"

    # Componenti
    HEADER = "#131922"
    SIDEBAR = "#10151c"
    CHAT_BG = "#0d1117"

    # Chat
    USER_MSG = "#0f766e"
    AI_MSG = "#171c24"

    # Testo
    TEXT_PRIMARY = "#f4f7fb"
    TEXT_SECONDARY = "#a7b0bd"
    TEXT_MUTED = "#6f7b8a"

    # Borders e Dividers
    BORDER_LIGHT = "#2a3441"
    BORDER_DARK = "#0d1117"
    DIVIDER = "#202a35"

    # Trasparenze
    GLASS_OPACITY_LOW = 0.5
    GLASS_OPACITY_MED = 0.7
    GLASS_OPACITY_HIGH = 0.9


class Fonts:
    # Primary font (Segoe UI per look professional)
    PRIMARY = "Segoe UI"

    # Fallback
    FALLBACK = "Arial"

    # Font sizes
    TITLE_LARGE = 28
    TITLE = 24
    HEADING = 16
    SUBTITLE = 12
    BODY = 14
    SMALL = 11
    TINY = 9

    # Weight styles
    BOLD = "bold"
    NORMAL = "normal"


class Styles:
    # Bordi arrotondati
    CORNER_SMALL = 8
    CORNER_MED = 12
    CORNER_LARGE = 16

    # Padding
    PAD_XS = 6
    PAD_SM = 12
    PAD_MD = 16
    PAD_LG = 20
    PAD_XL = 28

    # Ombre
    SHADOW_SM = "0 1px 2px rgba(0,0,0,0.05)"
    SHADOW_MD = "0 4px 6px rgba(0,0,0,0.1)"
    SHADOW_LG = "0 10px 15px rgba(0,0,0,0.2)"

    # Animazioni (ms)
    ANIM_FAST = 150
    ANIM_MED = 300
    ANIM_SLOW = 500


# Tema global per l'app
THEME = {
    "colors": Colors,
    "fonts": Fonts,
    "styles": Styles,
}
