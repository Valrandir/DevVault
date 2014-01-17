.PHONY: clean All

All:
	@echo ----------Building project:[ XGL_Demo - Debug ]----------
	@"$(MAKE)" -f "XGL_Demo.mk"
clean:
	@echo ----------Cleaning project:[ XGL_Demo - Debug ]----------
	@"$(MAKE)" -f "XGL_Demo.mk" clean
