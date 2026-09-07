# Centre Formation

Centre Formation is a Qt 6 desktop application for managing a training centre. It provides role-based workspaces for administrators, trainers, and trainees.

## Features

- Oracle database connection through Qt SQL and ODBC
- Administrator dashboard with rooms, trainers, courses, and trainees
- Separate permissions for super administrators and regular administrators
- Trainer course management and trainee attendance tracking
- Trainee progress, absence reports, room reservations, and certificates
- Incident reporting and administration resolution workflow
- PDF export for reports, attendance sheets, and certificates
- Optional local AI progress analysis through Ollama
- Email notifications through Brevo

## Requirements

- Qt 6 with Widgets, SQL, Charts, PrintSupport, and Network modules
- CMake 3.16 or newer
- C++17 compiler
- Oracle Database or Oracle XE
- Oracle ODBC driver
- Ollama, only if the local AI feature is used

## Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Run the generated executable from the selected build directory.

## Database Setup

1. Create the Oracle user and base tables required by the application.
2. Update the `[Database]` section in `settings.ini`.
3. Start Oracle and make sure the configured service is available.
4. Run `database/seed_demo_data.sql` if demo data is needed.

The application loads `database/schema.sql` from the Qt resource system to ensure that the reservation table exists.

## Demo Accounts

The demo seed file uses `password123` for its sample accounts:

- Super administrator: `owner@centrepro.tn`
- Administrator: `admin.operations@centrepro.tn`
- Trainer: `sarah.mansour@centrepro.tn`

Change these credentials for any real deployment.

## Local AI

The AI feature uses a local Ollama server. The configured model is defined in `settings.ini`.

```powershell
ollama run qwen2.5:3b
```

After Ollama is available, log in as a trainer, open `Suivi`, select a trainee, open `Analyse IA`, and start the analysis.

## Configuration and Security

`settings.ini` contains database credentials, email API credentials, and local service configuration. Do not publish real passwords or API keys in a public repository. Use local or deployment-specific configuration values instead.

## Project Structure

- `main.cpp`, `mainwindow.*`: application startup, login, and main navigation
- `db.*`: Oracle connection and schema initialization
- `authentification.*`: login and role handling
- `formateur.*`, `cours.*`, `salle.*`, `stagiaire.*`, `reservation.*`: database models
- `*widget.*`: administration management screens
- `roleworkspace.*`: trainer and trainee workspaces
- `moduletools.*`: shared UI, validation, charts, pagination, and PDF helpers
- `database/`: schema and demo data SQL files
