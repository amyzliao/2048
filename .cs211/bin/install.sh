#!/bin/sh

set -eu

main () {
    # Change to project root:
    cd "${1-$(dirname "$(dirname "$(dirname "$0")")")}"

    install_dot_idea
    install_dot_gitignore
}

install_dot_idea () {
    rm -Rf .idea
    hardlink_dir .cs211/idea .idea
}

install_dot_gitignore () {
    rm -f .gitignore
    ln -fv .cs211/.gitignore .gitignore
}

hardlink_dir () {
    NUL="$(printf '\0')"

    if [ -d "$1" ]; then
        mkdir "$2"

        find "$1" -depth 1 -print0 |
            while read -d "$NUL" item; do
                hardlink_dir "$item" "$2/${item##*/}"
            done
    else
        ln -fv "$1" "$2"
    fi
}

####
main
####
