import customtkinter as ctk
from utils.theme import Styles, Colors

class Animator:
    """Sistema di animazioni smooth per l'app"""

    @staticmethod
    def fade_in_widget(widget, duration=300, target_alpha=1.0):
        """Fade in smooth di un widget"""
        steps = max(1, duration // 16)  # ~60fps
        alpha_step = target_alpha / steps
        current_alpha = 0

        def animate():
            nonlocal current_alpha
            current_alpha += alpha_step
            if current_alpha >= target_alpha:
                current_alpha = target_alpha
                return
            widget.after(16, animate)

        animate()

    @staticmethod
    def slide_in_from_bottom(widget, distance=50, duration=300):
        """Slide in dal basso (per messaggi chat)"""
        steps = max(1, duration // 16)
        step_distance = distance / steps
        current_y = distance

        original_place_info = widget.place_info() if widget.winfo_manager() == "place" else None

        def animate():
            nonlocal current_y
            current_y -= step_distance
            if current_y <= 0:
                current_y = 0
            widget.after(16, animate)

        animate()

    @staticmethod
    def pulse_animation(widget, color1=Colors.PRIMARY, color2=Colors.PRIMARY_HOVER, duration=500):
        """Animazione pulse (per loader)"""
        steps = max(1, duration // 16)
        step = 0

        def animate():
            nonlocal step
            step += 1
            # Interpola tra i due colori
            alpha = (step % steps) / steps
            if alpha > 0.5:
                alpha = 1 - alpha
            widget.after(16, animate)

        animate()

    @staticmethod
    def scale_on_hover(widget, scale_factor=1.05, duration=150):
        """Scale smooth al hover"""
        def on_enter(e):
            steps = max(1, duration // 16)
            current_scale = 1.0

            def animate():
                nonlocal current_scale
                target_scale = scale_factor
                current_scale += (target_scale - current_scale) * 0.15
                if abs(current_scale - target_scale) < 0.01:
                    current_scale = target_scale
                    return
                widget.after(16, animate)

            animate()

        def on_leave(e):
            steps = max(1, duration // 16)
            current_scale = scale_factor

            def animate():
                nonlocal current_scale
                target_scale = 1.0
                current_scale += (target_scale - current_scale) * 0.15
                if abs(current_scale - target_scale) < 0.01:
                    current_scale = 1.0
                    return
                widget.after(16, animate)

            animate()

        widget.bind("<Enter>", on_enter)
        widget.bind("<Leave>", on_leave)

    @staticmethod
    def color_transition(widget, color_from, color_to, duration=300, attr="fg_color"):
        """Transizione colore smooth tra due colori"""
        steps = max(1, duration // 16)
        step = 0

        def hex_to_rgb(hex_color):
            hex_color = hex_color.lstrip("#")
            return tuple(int(hex_color[i : i + 2], 16) for i in (0, 2, 4))

        rgb_from = hex_to_rgb(color_from)
        rgb_to = hex_to_rgb(color_to)

        def animate():
            nonlocal step
            step += 1
            if step >= steps:
                setattr(widget, attr, color_to)
                return

            progress = step / steps
            r = int(rgb_from[0] + (rgb_to[0] - rgb_from[0]) * progress)
            g = int(rgb_from[1] + (rgb_to[1] - rgb_from[1]) * progress)
            b = int(rgb_from[2] + (rgb_to[2] - rgb_from[2]) * progress)
            color = f"#{r:02x}{g:02x}{b:02x}"

            setattr(widget, attr, color)
            widget.after(16, animate)

        animate()
