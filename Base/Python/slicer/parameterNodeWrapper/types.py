from .parameterPack import parameterPack

__all__ = [
    "FloatRange",
]

def _floatRangeInvariant(floatRange, minimum: float=None, maximum: float=None):
    minimum = minimum if minimum is not None else floatRange.minimum
    maximum = maximum if maximum is not None else floatRange.maximum

    if not minimum <= maximum:
        raise ValueError(
                f"The minimum of a range must be less than or equal to the maximum: {minimum} < {maximum}")

@parameterPack(invariant=_floatRangeInvariant)
class FloatRange:
    minimum: float
    maximum: float

    def setRange(self, minimum: float, maximum: float) -> None:
        self.setValues({
            "minimum": minimum,
            "maximum": maximum,
        })
