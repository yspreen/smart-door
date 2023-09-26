from http.server import BaseHTTPRequestHandler
from urllib.parse import parse_qs, urlparse

SECRET = "***REMOVED***"


class handler(BaseHTTPRequestHandler):
    def send_cors(self):
        self.send_header("Access-Control-Allow-Credentials", "true")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header(
            "Access-Control-Allow-Headers", "X-Requested-With, Content-type"
        )

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_cors()
        self.end_headers()

    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")

        self.send_cors()
        self.end_headers()
        html = """404""".encode()

        self.wfile.write(html)

    def do_POST(self):
        parsed_url = urlparse(self.path)
        params = parse_qs(parsed_url.query)

        key = params.get("key", [None])[0]

        if key == SECRET:
            self.handle_secret_key()

        self.send_response(302)  # Redirect
        self.send_header("Location", self.path)  # Redirect  which is handled by do_GET
        self.send_cors()
        self.end_headers()

    def handle_secret_key(self):
        from .redis import send_locked

        send_locked(False)
