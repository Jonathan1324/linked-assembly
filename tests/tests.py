from pathlib import Path
import logging
import importlib

dir = Path("tests")
log_dir = Path("logs")
logs = log_dir / "tests.log"
log_dir.mkdir(parents=True, exist_ok=True)

logger = logging.getLogger("tests")
logger.setLevel(logging.DEBUG)

file_handler = logging.FileHandler(str(logs), mode="w", encoding="utf-8")
file_handler.setLevel(logging.DEBUG)
file_formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")
file_handler.setFormatter(file_formatter)
logger.addHandler(file_handler)

console_handler = logging.StreamHandler()
console_handler.setLevel(logging.INFO)
console_formatter = logging.Formatter("[TEST:%(levelname)s] %(message)s")
console_handler.setFormatter(console_formatter)
logger.addHandler(console_handler)

def test(tools: list[str]) -> bool:
    for tool in tools:
        tool_dir = dir / tool
        tool_log_dir = log_dir / tool

        if not tool_dir.exists():
            logger.warning(f"{tool_dir} doesn't exist")
            continue

        tool_log_dir.mkdir(parents=True, exist_ok=True)

        test_file = tool_dir / "test.py"
        if not test_file.exists():
            logger.error(f"No test.py in {tool_dir}")
            continue

        try:
            spec = importlib.util.spec_from_file_location(f"{tool}_test", test_file)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)

            logger.debug(f"Starting tests for {tool}")

            if hasattr(mod, "test"):
                mod.test(tool_dir, tool_log_dir)
            else:
                logger.error(f"No test() function in {test_file}")
            
            logger.debug(f"Finished tests for {tool}")

        except Exception as e:
            logger.critical(f"Error while running tests for {tool}: {e}")

    return True

def clean() -> bool:
    return True
