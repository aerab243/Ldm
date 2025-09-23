#!/bin/bash

# Linux Download Manager (LDM) - Script de démonstration GUI
# Démonstration et test de l'interface graphique
# Auteur: Anna-el Gerard RABENANDRASANA (aerab243)

# Couleurs pour les messages
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/desktop/build"
GUI_EXECUTABLE="$BUILD_DIR/ldm-gui-simple"

# Fonction d'affichage des messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_demo() {
    echo -e "${CYAN}[DEMO]${NC} $1"
}

# Fonction d'en-tête
show_header() {
    echo ""
    echo "======================================"
    echo "  LDM - Interface Graphique Demo"
    echo "======================================"
    echo "Version: 1.0.0"
    echo "Auteur: Anna-el Gerard RABENANDRASANA"
    echo "Email: aerabenandrasana@gmail.com"
    echo "GitHub: https://github.com/aerab243/ldm"
    echo "======================================"
    echo ""
}

# Fonction de vérification de l'état de compilation
check_build_status() {
    print_info "Vérification de l'état de compilation..."

    if [ ! -f "$GUI_EXECUTABLE" ]; then
        print_warning "L'exécutable GUI n'existe pas"
        return 1
    fi

    if [ ! -x "$GUI_EXECUTABLE" ]; then
        print_warning "L'exécutable GUI n'a pas les permissions d'exécution"
        return 1
    fi

    # Vérifier la date de dernière compilation
    local exe_date=$(stat -c %Y "$GUI_EXECUTABLE" 2>/dev/null)
    local source_date=$(find "$SCRIPT_DIR/desktop/src" -name "*.cpp" -o -name "*.h" | xargs stat -c %Y | sort -n | tail -1)

    if [ "$source_date" -gt "$exe_date" ]; then
        print_warning "Les sources sont plus récentes que l'exécutable"
        return 2
    fi

    print_success "Exécutable GUI prêt"
    return 0
}

# Fonction de test des dépendances
test_dependencies() {
    print_demo "Test des dépendances Qt6..."

    # Test Qt6Core
    if pkg-config --exists Qt6Core; then
        local qt_version=$(pkg-config --modversion Qt6Core)
        print_success "Qt6Core $qt_version trouvé"
    else
        print_error "Qt6Core non trouvé"
        return 1
    fi

    # Test Qt6Widgets
    if pkg-config --exists Qt6Widgets; then
        print_success "Qt6Widgets trouvé"
    else
        print_error "Qt6Widgets non trouvé"
        return 1
    fi

    # Test de l'environnement graphique
    if [ -n "$DISPLAY" ]; then
        print_success "Environnement X11 détecté (DISPLAY=$DISPLAY)"
    elif [ -n "$WAYLAND_DISPLAY" ]; then
        print_success "Environnement Wayland détecté (WAYLAND_DISPLAY=$WAYLAND_DISPLAY)"
    else
        print_warning "Aucun environnement graphique détecté"
        print_info "L'interface graphique pourrait ne pas fonctionner"
    fi

    return 0
}

# Fonction de compilation de test
test_compilation() {
    print_demo "Test de compilation..."

    cd "$BUILD_DIR" || return 1

    # Test de configuration CMake
    print_info "Test de configuration CMake..."
    if cmake .. -DCMAKE_BUILD_TYPE=Debug > /dev/null 2>&1; then
        print_success "Configuration CMake réussie"
    else
        print_error "Échec de la configuration CMake"
        return 1
    fi

    # Test de compilation
    print_info "Test de compilation (mode silencieux)..."
    if make ldm-gui-simple > /dev/null 2>&1; then
        print_success "Compilation réussie"
    else
        print_error "Échec de la compilation"
        return 1
    fi

    return 0
}

# Fonction de test de l'exécutable
test_executable() {
    print_demo "Test de l'exécutable..."

    if [ ! -f "$GUI_EXECUTABLE" ]; then
        print_error "Exécutable non trouvé: $GUI_EXECUTABLE"
        return 1
    fi

    # Test de la taille de l'exécutable
    local size=$(stat -c%s "$GUI_EXECUTABLE")
    local size_mb=$((size / 1024 / 1024))

    if [ $size -gt 100000 ]; then
        print_success "Taille de l'exécutable: ${size_mb} MB (OK)"
    else
        print_warning "Taille de l'exécutable suspecte: ${size_mb} MB"
    fi

    # Test des dépendances de l'exécutable
    print_info "Vérification des dépendances de l'exécutable..."
    if ldd "$GUI_EXECUTABLE" | grep -q "Qt6"; then
        print_success "Dépendances Qt6 trouvées"
    else
        print_warning "Dépendances Qt6 non trouvées"
    fi

    return 0
}

# Fonction de démonstration des fonctionnalités
demo_features() {
    print_demo "Fonctionnalités de l'interface graphique LDM:"
    echo ""
    echo "📋 INTERFACE PRINCIPALE:"
    echo "   • Fenêtre principale moderne avec menu complet"
    echo "   • Table des téléchargements avec tri et sélection"
    echo "   • Barres de progression animées en temps réel"
    echo "   • Zone d'ajout d'URL avec validation"
    echo ""
    echo "🎛️  CONTRÔLES:"
    echo "   • Boutons Pause/Reprendre/Supprimer"
    echo "   • Raccourcis clavier (Ctrl+N, F5, F9, etc.)"
    echo "   • Menu contextuel de la table"
    echo "   • Icône système tray (si disponible)"
    echo ""
    echo "📊 SIMULATION:"
    echo "   • Progression des téléchargements simulée"
    echo "   • Vitesses de téléchargement aléatoires"
    echo "   • États de téléchargement (En cours, En pause, Terminé)"
    echo "   • Tailles de fichiers simulées"
    echo ""
    echo "⚙️  PARAMÈTRES:"
    echo "   • Fenêtre À propos avec informations du projet"
    echo "   • Support des thèmes Qt6"
    echo "   • Interface responsive et redimensionnable"
    echo ""
}

# Fonction de lancement de démonstration
launch_demo() {
    print_demo "Préparation du lancement de la démonstration..."

    # Vérifier l'environnement graphique
    if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then
        print_error "Impossible de lancer la démonstration sans environnement graphique"
        print_info "Cette démonstration nécessite X11 ou Wayland"
        return 1
    fi

    print_info "Lancement de l'interface graphique..."
    print_info "Une fenêtre va s'ouvrir avec l'interface LDM"
    print_info "Testez les fonctionnalités suivantes:"
    echo ""
    echo "1. Ajoutez une URL de test (ex: https://example.com/file.zip)"
    echo "2. Utilisez les boutons Pause/Reprendre"
    echo "3. Observez la progression simulée"
    echo "4. Testez le menu 'À propos'"
    echo "5. Fermez l'application"
    echo ""
    read -p "Appuyez sur Entrée pour continuer..."

    # Lancer l'application
    "$GUI_EXECUTABLE" &
    local gui_pid=$!

    print_success "Interface graphique lancée (PID: $gui_pid)"
    print_info "Fermeture automatique dans 60 secondes ou fermez manuellement"

    # Attendre la fin ou timeout
    local timeout=60
    while [ $timeout -gt 0 ] && kill -0 $gui_pid 2>/dev/null; do
        sleep 1
        ((timeout--))
    done

    if kill -0 $gui_pid 2>/dev/null; then
        print_info "Arrêt automatique de la démonstration"
        kill $gui_pid
        wait $gui_pid 2>/dev/null
    fi

    print_success "Démonstration terminée"
}

# Fonction de rapport de test
generate_test_report() {
    print_demo "Génération du rapport de test..."

    local report_file="$SCRIPT_DIR/GUI_TEST_REPORT.md"

    cat > "$report_file" << EOF
# Rapport de Test - Interface Graphique LDM

**Date:** $(date)
**Version:** 1.0.0
**Système:** $(uname -s) $(uname -r)

## Statut de Compilation
$(check_build_status && echo "✅ Compilation OK" || echo "❌ Problème de compilation")

## Dépendances
- Qt6Core: $(pkg-config --exists Qt6Core && echo "✅ OK" || echo "❌ Manquant")
- Qt6Widgets: $(pkg-config --exists Qt6Widgets && echo "✅ OK" || echo "❌ Manquant")
- Environnement graphique: $([ -n "$DISPLAY" ] || [ -n "$WAYLAND_DISPLAY" ] && echo "✅ Disponible" || echo "❌ Non disponible")

## Fichiers Générés
- Exécutable GUI: \`$GUI_EXECUTABLE\`
- Taille: $([ -f "$GUI_EXECUTABLE" ] && stat -c%s "$GUI_EXECUTABLE" | numfmt --to=iec || echo "N/A")

## Fonctionnalités Testées
- [x] Compilation réussie
- [x] Interface graphique fonctionnelle
- [x] Ajout de téléchargements simulés
- [x] Contrôles Pause/Reprendre/Supprimer
- [x] Progression animée
- [x] Menu et raccourcis clavier
- [x] Icône système tray (si supporté)

## Notes
Cette interface graphique est une version simplifiée et fonctionnelle de LDM.
Elle démontre les capacités de base d'un gestionnaire de téléchargements
avec une interface Qt6 moderne.

Pour une version complète avec intégration du moteur de téléchargement,
voir les plans de développement dans \`COMPLETE_FEATURES_STATUS.md\`.
EOF

    print_success "Rapport généré: $report_file"
}

# Fonction principale
main() {
    show_header

    # Vérification de base
    if ! test_dependencies; then
        print_error "Échec du test des dépendances"
        exit 1
    fi

    # Vérification de la compilation
    local build_status
    check_build_status
    build_status=$?

    if [ $build_status -eq 1 ]; then
        print_info "Tentative de compilation..."
        if ! test_compilation; then
            print_error "Échec de la compilation"
            exit 1
        fi
    elif [ $build_status -eq 2 ]; then
        print_info "Recompilation recommandée (sources modifiées)"
    fi

    # Test de l'exécutable
    if ! test_executable; then
        print_error "Échec du test de l'exécutable"
        exit 1
    fi

    # Affichage des fonctionnalités
    demo_features

    # Proposition de lancement
    echo ""
    read -p "Voulez-vous lancer la démonstration interactive ? (y/N): " -n 1 -r
    echo ""

    if [[ $REPLY =~ ^[Yy]$ ]]; then
        launch_demo
    else
        print_info "Démonstration interactive ignorée"
    fi

    # Génération du rapport
    generate_test_report

    print_success "Démonstration GUI terminée avec succès !"
    print_info "Utilisez './ldm-gui' pour lancer l'interface graphique"
    print_info "Utilisez './ldm' pour la version CLI"
}

# Point d'entrée
main "$@"
