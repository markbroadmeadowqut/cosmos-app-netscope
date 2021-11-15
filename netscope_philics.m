% CONFIG
remoteHost = 'cosmos-qut.local';  % hostname/IP address of server
remotePort = 8887;                % server port (default 8887)

% Setup
app = struct;
app.samples = 1024;
app.samplerate = 20e3;

% Signal definitions
signals = struct;

signals(1).name = 'Time (low)';
signals(1).index = 1;
signals(1).scale = 1;

signals(2).name = 'Time (high)';
signals(2).index = 2;
signals(2).scale = 1;

% Scope definitions
scopes = struct;

scopes(1).title = 'Timestamp';
scopes(1).ch(1).signals = [1 2];
scopes(1).ch(2).signals = [2];
scopes(1).columns = 1;
scopes(1).rows = 2;

% create scopes
for i = 1:length(scopes)
    scopes(i).scope = timescope( ...
        'Name', scopes(i).title, ...
        'SampleRate', app.samplerate, ...
        'TimeSpan', app.samples/app.samplerate, ...
        'BufferLength', app.samples, ...
        'LayoutDimensions', [scopes(i).rows, scopes(i).columns], ...
        'ChannelNames', {signals([scopes(1).ch(:).signals]).name}, ...
        'ShowLegend', true ...
    ); 
    scopes(i).scope.show;
end

mask = uint64(0);
for i = 1:length(signals)
    mask = bitset(mask,signals(i).index,1);
end

% create socket
server = tcpclient(remoteHost, remotePort);

write(server, 83, 'uint8');     % Header, 'N'
write(server, 86, 'uint8');     % Header, 'S'
write(server, mask, 'uint64');  
write(server, app.samples, 'uint32');

while(scopes(1).scope.isVisible)
    write(server, 1, 'uint8');
    data = read(server, app.samples*length(signals), 'uint16');
    data = double(reshape(data, length(signals), app.samples))';

    for i = 1:length(signals)
        data(:,i) = signals(i).scale*data(:,i); 
    end

    for i = 1:length(scopes)
        args = cell(1,length(scopes(i).ch));
        for j = 1:length(args)
            args(j) = {data(:,scopes(i).ch(j).signals)};
        end
        scopes(i).scope(args{:});
    end
end

% cleanup
clear server;


