all: build

format:
	find -iname *.h -o -iname *.cpp | xargs clang-format -i

build:
	platformio run

upload:
	platformio run --target upload

clean:
	platformio run --target clean


