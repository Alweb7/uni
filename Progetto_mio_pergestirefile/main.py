import os
import customtkinter as ctk
import threading
from tkinter import filedialog
from PIL import Image
from ai_brain import GeminiBrain
from file_inspector import FileInspector
from file_ops import FileLogic
from utils import Colors, Fonts, Styles, Animator

class App(ctk.CTk):
    def __init__(self):
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")
        super().__init__()
        self.title("TommyAI - Ollama Manager")
        self.geometry("1500x950")
        self.minsize(1100, 720)
        self.configure(fg_color=Colors.BG_DARK)

        try:
            self.iconbitmap("app_icon.ico")
        except:
            pass

        self.brain = GeminiBrain()
        self.current_path = os.getcwd()
        self.trash_path = FileLogic.inizializza_cestino()
        self.is_loading = False
        self.animation_queue = []
        self.loaded_file = None
        self.loaded_file_context = None

        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(1, weight=1)

        # --- HEADER ---
        self.header = ctk.CTkFrame(self, fg_color=Colors.HEADER, height=92, corner_radius=0)
        self.header.grid(row=0, column=0, columnspan=2, sticky="ew", padx=0, pady=0)
        self.header.grid_propagate(False)
        self.header.grid_columnconfigure(1, weight=1)

        brand = ctk.CTkFrame(self.header, fg_color="transparent")
        brand.grid(row=0, column=0, sticky="w", padx=Styles.PAD_LG, pady=Styles.PAD_SM)

        try:
            icon_source = "app_icon.png" if os.path.exists("app_icon.png") else "app_icon.ico"
            icon_img = Image.open(icon_source).resize((50, 50))
            self.icon_photo = ctk.CTkImage(light_image=icon_img, dark_image=icon_img, size=(50, 50))
            ctk.CTkLabel(brand, image=self.icon_photo, text="").pack(side="left", padx=(0, Styles.PAD_MD))
        except:
            ctk.CTkLabel(
                brand,
                text="T",
                width=50,
                height=50,
                corner_radius=14,
                fg_color=Colors.PRIMARY,
                font=(Fonts.PRIMARY, Fonts.TITLE, Fonts.BOLD),
                text_color=Colors.TEXT_PRIMARY,
            ).pack(side="left", padx=(0, Styles.PAD_MD))

        title_frame = ctk.CTkFrame(brand, fg_color="transparent")
        title_frame.pack(side="left")

        ctk.CTkLabel(title_frame, text="TommyAI", font=(Fonts.PRIMARY, Fonts.TITLE_LARGE, Fonts.BOLD), text_color=Colors.TEXT_PRIMARY).pack(anchor="w")
        ctk.CTkLabel(title_frame, text="Assistente locale per file, cartelle e pulizia", font=(Fonts.PRIMARY, Fonts.SUBTITLE, Fonts.NORMAL), text_color=Colors.TEXT_SECONDARY).pack(anchor="w")

        status = ctk.CTkFrame(self.header, fg_color="transparent")
        status.grid(row=0, column=1, sticky="e", padx=Styles.PAD_LG, pady=Styles.PAD_SM)

        ctk.CTkLabel(
            status,
            text="OLLAMA",
            fg_color=Colors.SURFACE_ALT,
            corner_radius=18,
            padx=14,
            pady=7,
            font=(Fonts.PRIMARY, Fonts.SMALL, Fonts.BOLD),
            text_color=Colors.PRIMARY,
        ).pack(side="right", padx=(Styles.PAD_SM, 0))
        ctk.CTkLabel(
            status,
            text=f"Modello: {self.brain.model_id}",
            font=(Fonts.PRIMARY, Fonts.SMALL),
            text_color=Colors.TEXT_SECONDARY,
        ).pack(side="right")

        # --- SIDEBAR ---
        self.sidebar = ctk.CTkFrame(self, width=360, fg_color=Colors.SIDEBAR, corner_radius=0)
        self.sidebar.grid(row=1, column=0, sticky="nsew", padx=0, pady=0)
        self.sidebar.grid_propagate(False)

        sidebar_title = ctk.CTkLabel(self.sidebar, text="File workspace", font=(Fonts.PRIMARY, Fonts.HEADING, Fonts.BOLD), text_color=Colors.TEXT_PRIMARY)
        sidebar_title.pack(pady=(Styles.PAD_LG, Styles.PAD_XS), padx=Styles.PAD_LG, anchor="w")

        self.path_lbl = ctk.CTkLabel(self.sidebar, text=self.current_path, wraplength=312, text_color=Colors.TEXT_SECONDARY, font=(Fonts.PRIMARY, Fonts.TINY), justify="left")
        self.path_lbl.pack(pady=Styles.PAD_SM, padx=Styles.PAD_LG, anchor="w")

        nav_frame = ctk.CTkFrame(self.sidebar, fg_color="transparent")
        nav_frame.pack(fill="x", padx=Styles.PAD_LG, pady=(0, Styles.PAD_SM))

        self._mini_button(nav_frame, "Home", lambda: self.vai_percorso(os.path.expanduser("~"))).pack(side="left", fill="x", expand=True, padx=(0, Styles.PAD_XS))
        self._mini_button(nav_frame, "Up", self.vai_su).pack(side="left", fill="x", expand=True, padx=(Styles.PAD_XS, 0))

        divider = ctk.CTkFrame(self.sidebar, fg_color=Colors.DIVIDER, height=1)
        divider.pack(fill="x", padx=Styles.PAD_MD, pady=Styles.PAD_SM)

        quick_title = ctk.CTkLabel(self.sidebar, text="Azioni rapide", font=(Fonts.PRIMARY, Fonts.SUBTITLE, Fonts.BOLD), text_color=Colors.TEXT_MUTED)
        quick_title.pack(padx=Styles.PAD_LG, pady=(Styles.PAD_SM, Styles.PAD_XS), anchor="w")

        quick_frame = ctk.CTkFrame(self.sidebar, fg_color="transparent")
        quick_frame.pack(fill="x", padx=Styles.PAD_LG, pady=(0, Styles.PAD_MD))
        quick_frame.grid_columnconfigure((0, 1), weight=1)

        self._quick_button(quick_frame, "Analizza", "analizza questa cartella", 0, 0)
        self._quick_button(quick_frame, "Ordina", "organizza questa cartella", 0, 1)
        self._quick_button(quick_frame, "Doppioni", "trova doppioni", 1, 0)
        self._quick_button(quick_frame, "Pulizia", "stima pulizia temp", 1, 1)

        file_tools = ctk.CTkFrame(self.sidebar, fg_color="transparent")
        file_tools.pack(fill="x", padx=Styles.PAD_LG, pady=(0, Styles.PAD_SM))
        self._mini_button(file_tools, "Carica file", self.carica_file_dialog).pack(side="left", fill="x", expand=True, padx=(0, Styles.PAD_XS))
        self._mini_button(file_tools, "Svuota", self.svuota_file_caricato).pack(side="left", fill="x", expand=True, padx=(Styles.PAD_XS, 0))

        self._mini_button(self.sidebar, "Apri cestino TommyAI", self.apri_cestino).pack(
            fill="x",
            padx=Styles.PAD_LG,
            pady=(0, Styles.PAD_SM),
        )

        self.loaded_file_lbl = ctk.CTkLabel(
            self.sidebar,
            text="Nessun file caricato",
            wraplength=312,
            font=(Fonts.PRIMARY, Fonts.TINY),
            text_color=Colors.TEXT_MUTED,
            justify="left",
        )
        self.loaded_file_lbl.pack(padx=Styles.PAD_LG, pady=(0, Styles.PAD_SM), anchor="w")

        self.file_count_lbl = ctk.CTkLabel(self.sidebar, text="", font=(Fonts.PRIMARY, Fonts.TINY), text_color=Colors.TEXT_MUTED)
        self.file_count_lbl.pack(padx=Styles.PAD_LG, pady=(0, Styles.PAD_XS), anchor="w")

        self.file_scroll = ctk.CTkScrollableFrame(self.sidebar, fg_color="transparent")
        self.file_scroll.pack(expand=True, fill="both", padx=Styles.PAD_SM, pady=Styles.PAD_SM)

        # --- MAIN CHAT AREA ---
        self.chat_container = ctk.CTkFrame(self, fg_color=Colors.CHAT_BG)
        self.chat_container.grid(row=1, column=1, sticky="nsew", padx=0, pady=0)
        self.chat_container.grid_columnconfigure(0, weight=1)
        self.chat_container.grid_rowconfigure(0, weight=1)

        self.chat_view = ctk.CTkScrollableFrame(self.chat_container, fg_color="transparent")
        self.chat_view.grid(row=0, column=0, sticky="nsew", padx=Styles.PAD_XL, pady=Styles.PAD_XL)

        # --- INPUT AREA ---
        self.input_frame = ctk.CTkFrame(self.chat_container, fg_color="transparent")
        self.input_frame.grid(row=1, column=0, sticky="ew", padx=Styles.PAD_XL, pady=Styles.PAD_LG)
        self.input_frame.grid_columnconfigure(0, weight=1)

        hint = ctk.CTkLabel(
            self.input_frame,
            text="Prova: vai in Downloads  |  carica un file e chiedi: riassumilo  |  organizza questa cartella",
            font=(Fonts.PRIMARY, Fonts.TINY),
            text_color=Colors.TEXT_MUTED,
        )
        hint.pack(anchor="w", padx=Styles.PAD_SM, pady=(0, Styles.PAD_XS))

        input_container = ctk.CTkFrame(
            self.input_frame,
            fg_color=Colors.SURFACE,
            corner_radius=Styles.CORNER_LARGE,
            border_width=1,
            border_color=Colors.BORDER_LIGHT,
        )
        input_container.pack(fill="x")

        self.entry = ctk.CTkEntry(
            input_container,
            placeholder_text="Scrivi un comando...",
            height=54,
            corner_radius=Styles.CORNER_MED,
            fg_color=Colors.SURFACE,
            text_color=Colors.TEXT_PRIMARY,
            placeholder_text_color=Colors.TEXT_MUTED,
            border_width=1,
            border_color=Colors.BORDER_LIGHT,
            font=(Fonts.PRIMARY, Fonts.BODY)
        )
        self.entry.pack(side="left", fill="both", expand=True, padx=Styles.PAD_LG, pady=Styles.PAD_SM)
        self.entry.bind("<Return>", lambda e: self.invia_async())

        self.btn = ctk.CTkButton(
            input_container,
            text="Invia",
            width=104,
            height=42,
            corner_radius=Styles.CORNER_MED,
            command=self.invia_async,
            font=(Fonts.PRIMARY, Fonts.SUBTITLE, Fonts.BOLD),
            fg_color=Colors.PRIMARY,
            hover_color=Colors.PRIMARY_HOVER,
            text_color=Colors.TEXT_PRIMARY
        )
        self.btn.pack(side="right", padx=Styles.PAD_SM, pady=Styles.PAD_SM)

        self.aggiorna_ui()
        self.aggiungi_bolla("Ciao Tommy! Sono TommyAI, il tuo assistente locale con Ollama. Sono pronto a gestire i tuoi file. Come posso aiutarti?", False)

    def _mini_button(self, parent, text, command):
        return ctk.CTkButton(
            parent,
            text=text,
            height=34,
            corner_radius=Styles.CORNER_SMALL,
            command=command,
            fg_color=Colors.SURFACE_ALT,
            hover_color=Colors.BORDER_LIGHT,
            text_color=Colors.TEXT_PRIMARY,
            font=(Fonts.PRIMARY, Fonts.SMALL, Fonts.BOLD),
        )

    def _quick_button(self, parent, text, command_text, row, column):
        button = ctk.CTkButton(
            parent,
            text=text,
            height=38,
            corner_radius=Styles.CORNER_MED,
            command=lambda: self.invia_testo(command_text),
            fg_color=Colors.BG_GLASS,
            hover_color=Colors.SURFACE_ALT,
            text_color=Colors.TEXT_PRIMARY,
            border_width=1,
            border_color=Colors.BORDER_LIGHT,
            font=(Fonts.PRIMARY, Fonts.SMALL, Fonts.BOLD),
        )
        button.grid(row=row, column=column, sticky="ew", padx=Styles.PAD_XS, pady=Styles.PAD_XS)
        return button

    def invia_testo(self, testo):
        if self.is_loading:
            return
        self.entry.delete(0, "end")
        self.entry.insert(0, testo)
        self.invia_async()

    def vai_percorso(self, path):
        if os.path.isdir(path):
            self.current_path = os.path.abspath(path)
            os.chdir(self.current_path)
            self.aggiorna_ui()
            self.aggiungi_bolla(f"Spostato in: {self.current_path}", False)

    def vai_su(self):
        parent = os.path.dirname(self.current_path)
        if parent and parent != self.current_path:
            self.vai_percorso(parent)

    def carica_file_dialog(self):
        path = filedialog.askopenfilename(title="Carica file in TommyAI")
        if path:
            self.carica_file(path)

    def carica_file(self, path):
        try:
            data = FileInspector.inspect(path)
            self.loaded_file = data
            self.loaded_file_context = data["context"]
            self.loaded_file_lbl.configure(text=f"File caricato: {data['name']}")
            self.aggiungi_bolla(data["summary"], False)
        except Exception as e:
            self.aggiungi_bolla(f"Non riesco a caricare il file: {e}", False)

    def svuota_file_caricato(self):
        self.loaded_file = None
        self.loaded_file_context = None
        self.loaded_file_lbl.configure(text="Nessun file caricato")
        self.aggiungi_bolla("Contesto file svuotato.", False)

    def apri_cestino(self):
        try:
            trash_path = FileLogic.inizializza_cestino()
            os.startfile(trash_path)
        except Exception as e:
            self.aggiungi_bolla(f"Non riesco ad aprire il cestino TommyAI: {e}", False)

    def aggiungi_bolla(self, testo, is_user):
        """Aggiungi messaggio chat con animazione"""
        side = "right" if is_user else "left"
        color = Colors.USER_MSG if is_user else Colors.AI_MSG
        text_color = Colors.TEXT_PRIMARY if is_user else Colors.TEXT_PRIMARY

        row = ctk.CTkFrame(self.chat_view, fg_color="transparent")
        row.pack(fill="x", pady=Styles.PAD_SM, padx=Styles.PAD_XS)

        bolla = ctk.CTkFrame(
            row,
            fg_color=color,
            corner_radius=Styles.CORNER_LARGE,
            border_width=0 if is_user else 1,
            border_color=Colors.BORDER_LIGHT,
        )
        bolla.pack(side=side, padx=Styles.PAD_SM)

        ctk.CTkLabel(
            bolla,
            text=testo,
            wraplength=760,
            padx=Styles.PAD_LG,
            pady=Styles.PAD_MD,
            font=(Fonts.PRIMARY, Fonts.BODY),
            text_color=text_color,
            justify="left"
        ).pack()

        # Animazione fade-in
        Animator.fade_in_widget(bolla, duration=Styles.ANIM_MED)

        try:
            self.chat_view._parent_canvas.yview_moveto(1.0)
        except:
            pass

    def aggiungi_indicatore_caricamento(self):
        """Loader con animazione pulse"""
        row = ctk.CTkFrame(self.chat_view, fg_color="transparent")
        row.pack(fill="x", pady=Styles.PAD_SM, padx=Styles.PAD_XS)

        loader = ctk.CTkFrame(row, fg_color=Colors.BG_GLASS, corner_radius=Styles.CORNER_LARGE)
        loader.pack(side="left", padx=Styles.PAD_SM)

        ctk.CTkLabel(
            loader,
            text="TommyAI sta elaborando...",
            padx=Styles.PAD_LG,
            pady=Styles.PAD_MD,
            font=(Fonts.PRIMARY, Fonts.BODY),
            text_color=Colors.PRIMARY
        ).pack()

        self.loader_frame = loader
        Animator.pulse_animation(loader, Colors.PRIMARY, Colors.PRIMARY_HOVER)

        try:
            self.chat_view._parent_canvas.yview_moveto(1.0)
        except:
            pass

    def rimuovi_indicatore_caricamento(self):
        """Rimuovi loader"""
        if hasattr(self, 'loader_frame'):
            self.loader_frame.pack_forget()
            self.loader_frame.destroy()

    def aggiorna_ui(self):
        """Aggiorna lista file nella sidebar"""
        for w in self.file_scroll.winfo_children():
            w.destroy()

        self.path_lbl.configure(text=self.current_path)

        try:
            hidden_items = {"__pycache__", ".tommyai_trash"}
            items = [item for item in os.listdir(self.current_path) if item not in hidden_items]
            items = sorted(items, key=lambda name: (not os.path.isdir(os.path.join(self.current_path, name)), name.lower()))
            dir_count = sum(1 for item in items if os.path.isdir(os.path.join(self.current_path, item)))
            file_count = len(items) - dir_count
            self.file_count_lbl.configure(text=f"{dir_count} cartelle, {file_count} file")

            max_visible = 350
            for item in items[:max_visible]:
                item_path = os.path.join(self.current_path, item)
                is_dir = os.path.isdir(item_path)
                prefix = "[DIR]" if is_dir else "[FILE]"
                color = Colors.SURFACE_ALT if is_dir else Colors.BG_GLASS

                file_btn = ctk.CTkButton(
                    self.file_scroll,
                    text=f"{prefix} {item}",
                    anchor="w",
                    fg_color=color,
                    hover_color=Colors.BORDER_LIGHT,
                    text_color=Colors.TEXT_PRIMARY,
                    font=(Fonts.PRIMARY, Fonts.BODY),
                    height=40,
                    corner_radius=Styles.CORNER_MED,
                    border_width=1,
                    border_color=Colors.BORDER_LIGHT,
                    command=(lambda p=item_path: self.vai_percorso(p)) if is_dir else (lambda p=item_path: self.carica_file(p)),
                )
                file_btn.pack(fill="x", pady=Styles.PAD_XS)

            if len(items) > max_visible:
                ctk.CTkLabel(
                    self.file_scroll,
                    text=f"...altri {len(items) - max_visible} elementi non mostrati per mantenere l'app veloce",
                    text_color=Colors.TEXT_MUTED,
                    font=(Fonts.PRIMARY, Fonts.TINY),
                    wraplength=300,
                    justify="left",
                ).pack(fill="x", pady=Styles.PAD_SM, padx=Styles.PAD_SM)
        except Exception as e:
            self.file_count_lbl.configure(text=f"Errore lettura cartella: {str(e)[:40]}")

    def invia_async(self):
        """Invia comando in thread separato (non blocca UI)"""
        if self.is_loading:
            return

        msg = self.entry.get().strip()
        if not msg:
            return

        self.is_loading = True
        self.btn.configure(state="disabled")
        self.entry.configure(state="disabled")

        self.aggiungi_bolla(msg, True)
        self.entry.delete(0, "end")
        self.aggiungi_indicatore_caricamento()

        thread = threading.Thread(target=self._elabora_comando, args=(msg,), daemon=True)
        thread.start()

    def _elabora_comando(self, msg):
        """Elabora comando in background"""
        try:
            res = self.brain.elabora_comando(msg, self.current_path, self.loaded_file_context)
            self.after(0, lambda: self._processa_risposta(res))
        except Exception as e:
            self.after(0, lambda: self._mostra_errore(str(e)))

    def _processa_risposta(self, res):
        """Processa risposta dall'AI"""
        self.rimuovi_indicatore_caricamento()
        try:
            self.current_path = FileLogic.esegui_azione(res, self.current_path)

            # Feedback per azione eseguita
            if res.azione == "vai_a":
                self.aggiungi_bolla(f"Spostato in: {self.current_path}", False)
            elif res.azione == "crea_cartella":
                self.aggiungi_bolla(f"Cartella '{res.nome}' creata con successo", False)
            elif res.azione == "crea_file":
                self.aggiungi_bolla(f"File '{res.nome}' creato con successo", False)
            elif res.azione == "elimina":
                self.aggiungi_bolla(f"'{res.nome}' eliminato con successo", False)

            # Sempre mostra la risposta principale
            if res.testo_risposta and res.testo_risposta.strip():
                self.aggiungi_bolla(res.testo_risposta, False)

            self.aggiorna_ui()
        except Exception as e:
            self.aggiungi_bolla(f"Errore: {str(e)}", False)
        finally:
            self.is_loading = False
            self.btn.configure(state="normal")
            self.entry.configure(state="normal")
            self.entry.focus()

    def _mostra_errore(self, errore):
        """Mostra errore in chat"""
        self.rimuovi_indicatore_caricamento()
        self.aggiungi_bolla(f"Errore di connessione: {errore}", False)
        self.is_loading = False
        self.btn.configure(state="normal")
        self.entry.configure(state="normal")
        self.entry.focus()

if __name__ == "__main__":
    App().mainloop()
