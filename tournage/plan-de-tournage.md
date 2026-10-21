# Plan de tournage n°1 : la boîte aux lettres connectée

Méthode générale et légende (H, V, É, ★) : voir [methode-de-tournage.md](methode-de-tournage.md).

## L'essentiel

- **Titre proposé** : « Ma boîte aux lettres m'envoie une PHOTO du courrier (ESP32, 15 €) »
- **Promesse** : savoir quand le facteur passe et voir ce qu'il a déposé, sans abonnement.
- **Durée visée** : 10 à 12 min. Les 3 étapes du projet deviennent 3 chapitres de la même vidéo.
- **À avoir prêt avant de tourner** : l'étape 1 (ESP32-C3) et l'étape 3 (ESP32-CAM avec carte SD) qui marchent, l'application ntfy sur le téléphone, une vraie boîte aux lettres accessible, une enveloppe et un petit colis, un multimètre, l'aimant et le capteur reed.
- **Besoin d'aide ?** Non, tu peux tout faire seul. Un complice qui met le courrier pendant que tu filmes le téléphone rend la démo finale plus naturelle.

## Script de la longue

| Temps | Chapitre | Ce qu'on voit | Ce que tu dis (voix off, à adapter) |
| --- | --- | --- | --- |
| 0:00 | Accroche | P01, P02, P03 | « Le facteur vient de passer. Je ne suis pas chez moi, mais je sais déjà ce qu'il a déposé. Et ça m'a coûté 15 euros. » |
| 0:20 | Le projet | P04, P05 | « Aujourd'hui on fabrique une boîte aux lettres connectée en trois étapes : une simple notification, puis une photo du courrier, puis un historique de tous les passages. Tout le code est en lien en description. » |
| 0:50 | Le matériel | P06, P07 | Présentation pièce par pièce, une phrase par pièce, prix à l'écran. |
| 2:00 | Étape 1 : la notification | P08 à P14 | Le capteur reed et son aimant, pourquoi « contact ouvert quand le clapet est fermé », le câblage, la batterie, ntfy, le premier « Tu as du courrier ! ». |
| 4:30 | Étape 2 : la photo | P15 à P20 | Pourquoi l'ESP32-CAM, la carte de programmation, le condensateur (l'astuce anti-redémarrage), le flash LED, la première photo reçue. |
| 7:00 | Étape 3 : l'archive | P21 à P24 | La carte SD, le journal daté, combien de fois le facteur passe vraiment. |
| 8:30 | Installation | P25 à P28 | Le boîtier étanche, la fixation dans la boîte, l'aimant sur le clapet. |
| 9:30 | La démo finale | P29 à P31 | Le vrai passage du courrier, filmé côté boîte et côté téléphone. |
| 10:30 | Autonomie et bonus | P32, P33 | Combien de temps tient la batterie, les idées bonus (e-mail, WhatsApp, panneau solaire). |
| 11:00 | Fin | P34 | « Le prochain projet, c'est la guirlande de Noël. Abonne-toi pour ne pas la rater, et dis-moi en commentaire ce que tu voudrais que ta boîte aux lettres fasse en plus. » |

## Liste des plans

| N° | Plan | Type | Note |
| --- | --- | --- | --- |
| P01 | Main qui glisse une enveloppe dans la boîte, de l'extérieur | ★ H+V | Plan d'ouverture : le geste du facteur |
| P02 | Téléphone posé sur une table, il vibre, la notification avec la photo apparaît | ★ H+V | Filme l'écran en vrai (pas en capture) pour voir la vibration |
| P03 | Gros plan de la photo du courrier reçue | É | Capture d'écran du téléphone |
| P04 | Le montage fini posé sur l'établi, travelling lent | H | |
| P05 | Les 3 étapes posées côte à côte (C3, CAM, CAM + SD) | H | Pour le schéma « 3 étapes » |
| P06 | Toutes les pièces étalées, puis apparition une par une | H | Pose les pièces une à une, caméra fixe, coupe chaque pose |
| P07 | Gros plan ESP32-C3 SuperMini à côté d'une pièce de 1 € | ★ H+V | Montre la taille |
| P08 | Capteur reed + aimant : approcher, éloigner | H | |
| P09 | Multimètre en mode continuité sur le reed, l'aimant approche, le bip s'arrête | ★ H+V | Explique le sens du capteur, très bon plan pédagogique |
| P10 | Câblage du reed sur l'ESP32-C3, en accéléré | H | Vue au-dessus |
| P11 | Batterie 18650, TP4056, HT7333 : câblage | H | Mentionne la batterie de marque |
| P12 | `config.h` dans VS Code : Wi-Fi et sujet ntfy (masque les vrais codes) | É | Floute le mot de passe au montage |
| P13 | Téléversement `pio run -t upload` et moniteur série | É | |
| P14 | Aimant éloigné à la main, notification « Tu as du courrier ! » | ★ H+V | Premier succès, garde ta réaction |
| P15 | ESP32-CAM, gros plan de l'objectif et de la LED flash | H | |
| P16 | Branchement sur la carte ESP32-CAM-MB | H | |
| P17 | Soudure ou pose du condensateur 1000 µF | H | Explique : les pics de courant du Wi-Fi |
| P18 | Le flash qui s'allume dans une boîte en carton fermée (filmé par une ouverture) | ★ H+V | Visuel fort |
| P19 | La photo arrive dans ntfy | É | |
| P20 | Comparaison photo avec et sans flash | É | Écran partagé au montage |
| P21 | Insertion de la carte micro SD | H | |
| P22 | Carte SD lue sur l'ordinateur : dossier de photos | É | |
| P23 | Ouverture du journal daté | É | Prépare quelques jours de passages avant |
| P24 | Graphique ou liste des heures de passage du facteur | É | Fais-le dans un tableur, c'est une super anecdote |
| P25 | Le montage glissé dans le boîtier étanche | H | |
| P26 | Fixation dans la boîte aux lettres, intérieur | H | Téléphone dans la boîte, lampe d'appoint |
| P27 | Collage de l'aimant sur le clapet, alignement avec le capteur | H | |
| P28 | Plan large de la boîte aux lettres, rue ou jardin | H+V | Plan d'ambiance |
| P29 | Démo finale : le courrier tombe dans la boîte (vue de l'intérieur si possible) | ★ H+V | |
| P30 | Démo finale : le téléphone reçoit la photo (même moment, deuxième caméra) | ★ H+V | Clap pour synchroniser |
| P31 | La photo prise par la boîte, plein écran | É | |
| P32 | Mesure de consommation au multimètre en sommeil profond | H | Chiffre fort pour l'autonomie |
| P33 | Panneau solaire posé à côté (même non câblé) | H | Teaser du bonus |
| P34 | Boîte aux lettres, plan large qui s'éloigne | H | Plan de fin |

## Les 5 Shorts

Chaque Short dure 30 à 60 s, s'ouvre sur un plan vertical ★ et se termine par un renvoi vers la longue (« Le tuto complet est en vidéo associée »).

| N° | Accroche (premières 2 s, en texte à l'écran + voix) | Plans | Déroulé |
| --- | --- | --- | --- |
| S1 | « Ma boîte aux lettres m'envoie une photo du courrier » | P01, P02, P03, P18, P31 | Le résultat d'abord, puis « 15 € et un ESP32 » et un aperçu rapide du montage. C'est le Short « vitrine », à publier le premier. |
| S2 | « Le facteur passe à quelle heure, vraiment ? » | P28, P24, P23, P21 | La carte SD a tout noté pendant X jours : le résultat surprenant (heure moyenne, jour sans courrier). |
| S3 | « Ce composant à 1 € détecte l'ouverture de ta boîte aux lettres » | P08, P09, P14 | Le reed expliqué en 40 s avec le bip du multimètre. Short pédagogique, bon pour les recherches. |
| S4 | « Mon ESP32-CAM redémarrait sans arrêt. La solution coûte 20 centimes » | P15, P17, P18, P19 | L'astuce du condensateur. Si tu as filmé un vrai plantage pendant la mise au point, c'est l'ouverture idéale. |
| S5 | « Un ESP32 plus petit qu'une pièce de 1 € » | P07, P10, P14, P32 | La SuperMini, son prix, et l'autonomie en sommeil profond. |

## Checklist du jour J

- Batterie chargée, carte SD vidée puis réinsérée (avec quelques jours de journal déjà enregistrés pour P23-P24).
- Notifications du téléphone désactivées sauf ntfy, mode avion avec Wi-Fi.
- Masquer le nom du Wi-Fi, l'adresse et le numéro de rue sur tous les plans.
- Enveloppe neutre, sans ton nom ni ton adresse.
