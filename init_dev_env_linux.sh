#!/bin/bash

set -ex

script_path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

git config --global user.name "Xu Zhang"
git config --global user.email "simonzgx@gmail.com"

sudo apt update
sudo apt -y install zsh cmake
sh -c "$(curl -fsSL https://raw.github.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"