import logging
import re
import os
import azure.functions as func
from azure.data.tables import TableClient

def is_valid_email(email: str) -> bool:
    """
    Validates the email address using a basic regex pattern.
    """
    email_regex = r'^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$'
    return bool(re.match(email_regex, email))

def main(req: func.HttpRequest) -> func.HttpResponse:
    try:
        req_body = req.get_json()  # Extract JSON from the request body
        email = req_body.get('email')  # Get the email address from the JSON
    except ValueError:
        # If the body isn't valid JSON
        return func.HttpResponse(
            "Invalid JSON provided in the request body.",
            status_code=400
        )

    # Check if the email is provided
    if not email:
        return func.HttpResponse(
            "Please provide an email address in the 'email' field.",
            status_code=400
        )

    # Validate the email format
    if not is_valid_email(email):
        return func.HttpResponse(
            "Invalid email address format.",
            status_code=400
        )
    

    CONNECTION_STRING = os.getenv("AzureEmailSettings")

    table_client = TableClient.from_connection_string(conn_str=CONNECTION_STRING, table_name="EmailsList")

    email_entity = {
    "PartitionKey": "Emails",
    "RowKey": email,
    "EmailAddress": email,
    "DateAdded": str(func.datetime.datetime.utcnow())
    }

    # Insert the entity into the table
    table_client.create_entity(entity=email_entity)

    # If everything is fine, send a success response
    return func.HttpResponse(
        f"Received email address: {email}",
        status_code=200
    )