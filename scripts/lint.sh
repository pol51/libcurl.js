set -e

find lib tests -name '*.h' -o -name '*.c' -o -name '*.cpp' | xargs clang-format -Werror --dry-run
pnpm run lint
pnpm run check
