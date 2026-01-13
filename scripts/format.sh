find lib tests -name '*.h' -o -name '*.c' -o -name '*.cpp' | xargs clang-format -i
pnpm run lint:fix
