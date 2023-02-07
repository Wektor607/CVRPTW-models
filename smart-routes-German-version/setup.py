from pathlib import Path

from setuptools import (
    find_packages,
    setup,
)

from smart_routes import __version__

this_directory = Path(__file__).parent
long_description = (this_directory / 'README.md').read_text()

setup(
    name='smart-routes',
    version=__version__,
    description='Smart Routes Optimization',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='sergun@gmail.com',
    license='free for now',
    url='https://gitlab.com/sergun/smart-routes',
    packages=find_packages(),
    install_requires=[
        'pydantic',
    ],
    include_package_data=True,
    python_requires=">=3.8",
)
