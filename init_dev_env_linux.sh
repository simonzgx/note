#!/bin/bash

set -ex

script_path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

git config --global user.name "Xu Zhang"
git config --global user.email "simonzgx@gmail.com"

sudo apt update
sudo apt -y install zsh cmake ninja-build clangd ccache lld clang-format clang
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

