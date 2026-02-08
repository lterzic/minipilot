import pytest

class Info:
    def __init__(self, test_name):
        self.test_name = test_name

@pytest.fixture
def info(request):
    return Info(request.path.parent.name)