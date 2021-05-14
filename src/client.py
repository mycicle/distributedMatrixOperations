import sys
import attr
import json
import enum
import traceback
import numpy as np


from enum import Enum
from flask import Flask
from loguru import logger
# known issue https://github.com/flask-restful/flask-restful/pull/913
import flask.scaffold
flask.helpers._endpoint_from_view_func = flask.scaffold._endpoint_from_view_func
from flask_restful import Resource, Api, reqparse

app = Flask(__name__)
api = Api(app)

@enum.unique
class Operator(Enum):
    multiply = "multiply"
    dot = "dot"
    add = "add"
    subtract = "subtract"

    @classmethod
    def has_value(cls, value):
        """
        Will check for consistency of the right hand side of the enum values
        ex: multiply = 'mul', this function will return True if value == 'mul'
        """
        for pair in cls.__members__.values():
            if value == pair.value:
                return True
        return False

@attr.s(frozen=False)
class Worker():
    operator: str = attr.ib(validator=attr.validators.instance_of(str))
    left_operand: np.ndarray = attr.ib(validator=attr.validators.instance_of(np.ndarray), order=False, hash=False)
    right_operand: np.ndarray = attr.ib(validator=attr.validators.instance_of(np.ndarray), order=False, hash=False) 
    
    @operator.validator
    def is_valid_operand(self, attribute, value):
        if not Operator.has_value(value):
            raise ValueError(f"Operator should be in {list(Operator)}")

    def np_to_json(self, array):
        return json.dumps(array.tolist())
    def compute(self):
        output = {}

        try:
            if self.operator == Operator.dot.value:
                output["answer"] = self.np_to_json(np.dot(self.left_operand, self.right_operand))
            elif self.operator == Operator.multiply.value:
                output["answer"] = self.np_to_json(np.multiply(self.left_operand, self.right_operand))
            elif self.operator == Operator.add.value:
                output["answer"] = self.np_to_json(np.add(self.left_operand, self.right_operand))
            elif self.operator == Operator.subtract.value:
                output["answer"] = self.np_to_json(np.subtract(self.left_operand, self.right_operand))

        except Exception as err:
            traceback.print_exc(file=sys.stderr)
            output["error"] = traceback.format_exc()
            return output, 400

        return output, 200


@attr.s(frozen=True)
class Client(Resource):
    __id: str = "asdf" # placeholder
    @property
    def id(self):
        return self.__id
    
    def post(self):
        parser = reqparse.RequestParser()

        parser.add_argument('operator', required=True)
        parser.add_argument('left_operand', required=True)
        parser.add_argument('right_operand', required=True)

        args = parser.parse_args()

        # Create a worker object to validate arguments and do the logic
        try:
            worker = Worker(args.operator, np.array(json.loads(args.left_operand)), np.array(json.loads(args.right_operand)))
            return worker.compute()
        except Exception as err:
            traceback.print_exc(file=sys.stderr)
            return {"error": err}, 400

if __name__ == "__main__":
    # worker = Worker("1234", "subtract", np.array([1]), np.zeros(5))
    api.add_resource(Client, '/client')
    app.run()
