.PHONY: clean All

All:
	@echo ----------Building project:[ lem1802 - Debug ]----------
	@cd "src/lem1802" && "$(MAKE)" -f "lem1802.mk"
clean:
	@echo ----------Cleaning project:[ lem1802 - Debug ]----------
	@cd "src/lem1802" && "$(MAKE)" -f "lem1802.mk" clean
