%% BEGIN USER DEFINED PARAMETERS %%

% Remote controller configuration
remoteHost = 'cosmos-qut.local';  % hostname/IP address of server
remotePort = 8887;                % server port (default 8887)

% Application parameters
app = struct;
app.samples = 1024;     % Windows length in sampels for each capture
app.samplerate = 20e3;  % Sample rate of data (for time base scaling)

% SIGNAL DEFINITIONS
% You may define here 1+ signals to be sampled from the remote controller.
% The signal name is used only for dsiplay in scope legends (and to make
% this configuration more readable). The type, offset, and scale
% parameters control how the raw signals are processed prior to display.
% It is also possible to specify different processing for the same signal.
% value = (typecast(signal, type) + offset)*scale;
signals = struct;

signals(1).name = 'Time (low)'; % Name used in scope legend
signals(1).index = 1;           % Index in SV vector on SoC
signals(1).type = 'uint16';     % Data type of 16-bit word (typ. uint16 or int16)
signals(1).offset = -32768;     % Offset
signals(1).scale = 1/32768;     % Scaling factor

signals(2).name = 'Time (high)';
signals(2).index = 2;
signals(2).type = 'int16';
signals(2).offset = 0;
signals(2).scale = 1;

signals(3).name = 'Time (low)';
signals(3).index = 1;
signals(3).type = 'int16';
signals(3).offset = 0;
signals(3).scale = 1;

% SCOPE DEFINITIONS
% You may define 1+ scopes here, each with 1+ channels (axes)
% Each channel may display an arbitrary collections of the 
% signals defined in the signals struct above. The channel
% indexes used should not exceed (columns x rows);
scopes = struct;

scopes(1).title = 'Timestamp';      % Scope title
scopes(1).ch(1).signals = [1];    % Signal index vector (from signals struct) to display in each channel
scopes(1).ch(2).signals = [2 3];
scopes(1).columns = 1;              % Arrangement of scope axes
scopes(1).rows = 2;

%% END USER DEFINED PARAMETERS %%

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

% Construct SV bitmask based on specified signals
mask = uint64(0);
for i = 1:length(signals)
    mask = bitset(mask,signals(i).index,1);
end

% create socket
server = tcpclient(remoteHost, remotePort);

% write netscope config header
write(server, 83, 'uint8');     % Header, 'N'
write(server, 86, 'uint8');     % Header, 'S'
write(server, mask, 'uint64');  
write(server, app.samples, 'uint32');

% preallocate processed signal array
data_disp = zeros(app.samples, length(signals)); 

while(scopes(1).scope.isVisible)
    % Request one new window of data
    write(server, 1, 'uint8');

    % Get data and reshape into matrix
    data = read(server, app.samples*length(unique([signals.index])), 'uint16');
    data = reshape(data, length(unique([signals.index])), app.samples)';

    % Process type, offset, scaling
    for i = 1:length(signals)
        data_disp(:,i) = (double(typecast(data(:,signals(i).index), signals(i).type))+signals(i).offset)*signals(i).scale; 
    end

    % For each scope, allocate data into cell array corresponding to channels
    for i = 1:length(scopes)
        args = cell(1,length(scopes(i).ch));
        for j = 1:length(args)
            args(j) = {data_disp(:,scopes(i).ch(j).signals)};
        end
        % Plot data in scope
        scopes(i).scope(args{:});
    end
end

% cleanup
clear server;


