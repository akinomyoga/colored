#!/usr/bin/env bash

function sub:install-colored {
  local src=$1 dst=$2

  local ls_help=$(ls --help)

  if grep -qFe '--time-style=' <<< "$ls_help"; then
    ls_longopt_time_style=1
  else
    ls_longopt_time_style=
  fi

  if grep -qFe '--color=' <<< "$ls_help"; then
    ls_longopt_color=1
  else
    ls_longopt_color=
  fi

  sed '
    s"^\(colored_share_directory=\).*"\1'"$share_directory"'"
    s"^\(ls_has_longopt_time_style=\).*"\1'"$ls_longopt_time_style"'"
    s"^\(ls_has_longopt_color=\).*"\1'"$ls_longopt_color"'"
  ' "$src" > "$dst"

  chmod +x "$dst"
}

function sub:update {
  # remove binaries from old modls
  local name
  for name in l ll la; do
    [[ -x ~/.mwg/bin/$name ]] && rm ~/.mwg/bin/"$name"
  done
  [[ -d ~/.mwg/share/modls ]] &&
    rm -r ~/.mwg/share/modls
  return 0
}

if declare -f "sub:$1" &>/dev/null; then
  "sub:$@"
else
  return 1
fi
